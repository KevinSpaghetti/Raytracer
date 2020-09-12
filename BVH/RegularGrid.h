//
// Created by kevin on 10/09/20.
//

#pragma once

#include <vector>
#include <algorithm>
#include "../Utils/Triangle.h"
#include "AABB.h"

class RegularGrid : public Boxable, public IntersectTestable {
public:
    //TODO: Avoid expensive copies of the vectors using pointers
    RegularGrid(std::vector<Triangle> triangles,
                std::vector<Vertex> vertices,
                std::vector<Normal> normals,
                std::vector<UV> uvs) :
                triangles(triangles),
                vertices(vertices),
                normals(normals),
                uvs(uvs) {
        //Build the grid
        //Insert every triangle in all the cells
        box = AABB();

        std::vector<TriangleInfo> tinfo;

        int index = 0;
        for(Triangle t : triangles) {
            Vertex v1 = vertices[t.vta];
            Vertex v2 = vertices[t.vtb];
            Vertex v3 = vertices[t.vtc];
            AABB tbox = AABB(std::vector({v1, v2, v3}));
            box = AABB(make_shared<AABB>(box), make_shared<AABB>(tbox));
            //Add the triangle info to the cells
            tinfo.push_back({
                    index,
                    t,
                    tbox
            });
        }

        Point grid_size = box.getMax() - box.getMin();
        float multiplier = 4;
        float scale = powf((multiplier * triangles.size())/(grid_size.x * grid_size.y * grid_size.z), 1.0f/3.0f);
        resolution = glm::floor(grid_size * scale);
        resolution = glm::max(Point(1), glm::min(resolution, Point(128)));
        cell_extent = grid_size / resolution;

        //Reserve the space for the cells
        cells = std::vector<Cell>(resolution.x * resolution.y * resolution.z);
        for (Cell& c : cells){
            c = Cell{
                std::list<TriangleInfo>()
            };
        }

        //Insert all the triangles in all the cells in which they are contained
        for(TriangleInfo t: tinfo){
            Point min = t.box.getMin();
            Point max = t.box.getMax();

            //Convert in grid cell coordinates
            min = (min - box.getMin()) / cell_extent;
            max = (max - box.getMin()) / cell_extent;

            //Min and Max cells that the bounding box overlap
            //dmin and dmax define a sub-box that contains the bounding box of the triangle
            Point dmin = glm::clamp(glm::floor(min), Point(0), resolution - 1.0f);
            Point dmax = glm::clamp(glm::floor(max), Point(0), resolution - 1.0f);

            for (int z = dmin.z; z <= dmax.z; ++z) {
                for (int y = dmin.y; y <= dmax.y; ++y) {
                    for (int x = dmin.x; x <= dmax.x; ++x) {
                        int cell_index = z * resolution.x * resolution.y + y * resolution.x + x;
                        cells[cell_index].overlaps.emplace_back(t);
                    }
                }
            }


        }

    }

    //Maybe return a list of intersections
    std::list<Intersection> intersect(const Ray& r) const {
        std::list<Intersection> ins;
        if(!box.isHit(r)){
            return ins;
        }

        Point cell; //The cell in which we are
        Point step, exit;
        Point deltaT, nextCrossingT;

        //Find the coordinates of the cell in which we sould start traversing
        //the grid
        //If the ray is inside we find the cell
        //If the ray is outside we find the first cell that it hits
        float tmin, tmax;
        box.getHitPoint(r, tmin, tmax);

        Point ray_origin_cell = (r.getOrigin() + tmin * r.getDirection()) - box.getMin();
        cell = glm::clamp(glm::floor(ray_origin_cell / cell_extent), Point(0), resolution - 1.0f);
        Point inv_ray_dir = (1.0f / r.getDirection());
        for (int i = 0; i < 3; ++i) {
            if (r.getDirection()[i] < 0) {
                deltaT[i] = -cell_extent[i] * inv_ray_dir[i];
                nextCrossingT[i] = tmin + (cell[i] * cell_extent[i] - ray_origin_cell[i]) * inv_ray_dir[i];
                exit[i] = -1;
                step[i] = -1; //If the ray is pointing backwards we need to take a step back
            }
            else {
                deltaT[i] = cell_extent[i] * inv_ray_dir[i];
                nextCrossingT[i] = tmin + ((cell[i] + 1)  * cell_extent[i] - ray_origin_cell[i]) * inv_ray_dir[i];
                exit[i] = resolution[i];
                step[i] = 1;
            }
        }

        bool finished = false;

        while (!finished) {
            int id = cell.z * resolution.x * resolution.y + cell.y * resolution.x + cell.x;
            for (TriangleInfo t : cells[id].overlaps) {
                Vertex v1 = vertices[t.tri.vta];
                Vertex v2 = vertices[t.tri.vtb];
                Vertex v3 = vertices[t.tri.vtc];
                float ts, u, v;
                //TODO: refactor to own function
                if(intersections::intersect(r, v1, v2, v3, ts, u, v)){
                    //TODO: Check the normalization of all the directions
                    //Interpolate the informations from all the vertices
                    Vertex ip{0, 0, 0};
                    //Worse method, see raytracing gems 3
                    //Gets progressively worse with more camera distance
                    //ip = r.getOrigin() + glm::normalize(r.getDirection()) * t;
                    ip = (1.0f-u-v) * v1 + u * v2 + v * v3; //Does not depend on the direction normalization
                    Normal nm{0, 0, 0};
                    UV uv{0, 0, 0};
                    //Check if the normals are there
                    if(!normals.empty()){
                        Normal n1 = normals[t.tri.nma];
                        Normal n2 = normals[t.tri.nmb];
                        Normal n3 = normals[t.tri.nmc];
                        nm = glm::normalize((1.0f-u-v) * n1 + u * n2 + v * n3);
                    }
                    if(!uvs.empty()){
                        UV uv1 = uvs[t.tri.uva];
                        UV uv2 = uvs[t.tri.uvb];
                        UV uv3 = uvs[t.tri.uvc];
                        uv = (1.0f-u-v) * uv1 + u * uv2 + v * uv3;
                        uv.y = (1.0 - uv.y); //Invert the y axis
                    }

                    ins.push_back({ip, nm, uv});
                }
            }
            int k = ((nextCrossingT[0] < nextCrossingT[1]) << 2) +
                    ((nextCrossingT[0] < nextCrossingT[2]) << 1) +
                    ((nextCrossingT[1] < nextCrossingT[2]));
            static const int map[8] = {2, 1, 2, 1, 2, 2, 0, 0};
            int axis = map[k];
            //We hit a triangle and reached the end of the cell
            if (tmax < nextCrossingT[axis]) {
                finished = true;
            }
            cell[axis] += step[axis];
            //We got out of the grid
            if (cell[axis] == exit[axis]) {
                finished = true;
            }
            nextCrossingT[axis] += deltaT[axis];
        }

        return ins;
    }


    //TODO: Refactor, Use a pointer for the BBox
    shared_ptr<BoundingBox> getSurroundingBox() override {
        return static_cast<shared_ptr<BoundingBox>>(&box);
    }

private:

    struct TriangleInfo{
        int triangle_index;
        Triangle tri;
        AABB box;
    };

    struct Cell{
        std::list<TriangleInfo> overlaps;
    };


private:
    std::vector<Triangle> triangles;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<UV> uvs;

    Point resolution; //Number of cells in each dimension
    Point cell_extent; //dimension of a cell

    std::vector<Cell> cells;
    AABB box;
};
