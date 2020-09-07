//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Mesh.h"

class TriangleMesh : public Mesh {
public:
    TriangleMesh() : vertices(),
             triangles(),
             normals(),
             uvs() {}

    TriangleMesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, std::vector<Normal>& normals, std::vector<UV>& uvs) :
    vertices(vertices),
    triangles(triangles),
    normals(normals),
    uvs(uvs){}

    std::list<Intersection> intersect(const Ray& r) const override {
        std::list<Intersection> ins;
        for (Triangle tri : triangles) {
            Vertex v1 = vertices[tri.vta];
            Vertex v2 = vertices[tri.vtb];
            Vertex v3 = vertices[tri.vtc];

            float t, u, v;
            if(intersections::intersect(r, v1, v2, v3, t, u, v)){
                //TODO: Check the normalization of all the directions
                //Interpolate the informations from all the vertices
                Vertex ip{0, 0, 0};
                //ip = r.getOrigin() + glm::normalize(r.getDirection()) * t;
                //Does not depend on the direction normalization
                ip = (1.0f-u-v) * v1 + u * v2 + v * v3;
                Normal nm{0, 0, 0};
                UV uv{0, 0, 0};
                //Check if the normals are there
                if(hasNormals()){
                    Normal n1 = normals[tri.nma];
                    Normal n2 = normals[tri.nmb];
                    Normal n3 = normals[tri.nmc];
                    nm = glm::normalize((1.0f-u-v) * n1 + u * n2 + v * n3);
                }

                if(hasUVs()){
                    UV uv1 = uvs[tri.uva];
                    UV uv2 = uvs[tri.uvb];
                    UV uv3 = uvs[tri.uvc];
                    uv = (1.0f-u-v) * uv1 + u * uv2 + v * uv3;
                    uv.y = (1.0 - uv.y); //Invert the y axis
                }

                ins.push_back({ip, nm, uv});

            }
        }

        return ins;
    }

    shared_ptr<BoundingBox> getSurroundingBox(){
        if(!hasVertices()) {
            //Return a bounding box for all the scene
            return std::make_shared<AABB>(Point{0, 0, 0}, Point{0, 0, 0});
        }

        Point min{consts::infinity, consts::infinity, consts::infinity};
        Point max{-consts::infinity, -consts::infinity, -consts::infinity};

        for(Vertex v : vertices){
            for (int i = 0; i < 3; ++i) {
                if (v[i] < min[i]){
                    min[i] = v[i];
                }
                if (v[i] > max[i]){
                    max[i] = v[i];
                }
            }
        }

        //Add some thickness to the box in case the triangles lie on the same plane
        //the box would be with 0 thickness and the ray would never hit it
        std::shared_ptr<BoundingBox> bbox = std::make_shared<AABB>(min - Point{0.1, 0.1, 0.1}, max + Point{0.1, 0.1, 0.1});

        return bbox;
    }

    bool hasNormals() const {
        return !normals.empty();
    }
    bool hasUVs() const {
        return !uvs.empty();
    }
    bool hasVertices() const {
        return !vertices.empty();
    }
    bool hasTriangles() const {
        return !triangles.empty();
    }

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;
};
