//
// Created by kevin on 04/09/20.
//

#pragma once
#include <algorithm>

#include "Mesh.h"
#include "../Utils/Triangle.h"
#include "../AccelerationMethods/RegularGrid.h"
#include "../AccelerationMethods/ExhaustiveSearch.h"
#include "TriangleMeshData.h"

//class that implements the concept of a triangle mesh, composed by the triangle mesh
//data, logic and intersection acceleration structures
class TriangleMesh : public Mesh {
public:
    TriangleMesh() = delete;

    //We want to copy the vectors to avoid modifications from third parties
    TriangleMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles, std::vector<Normal> normals, std::vector<UV> uvs) :
        data(std::make_shared<TriangleMeshData>(vertices, triangles, normals, uvs)),
        box(buildSurroundingBox()),
        accelerator(std::make_unique<ExhaustiveSearch>(data)){}

    void buildAccelerationStructure(){
        std::cout << "Building acceleration structure\n";
        accelerator = std::make_unique<RegularGrid>(data);
        std::cout << "Done\n";
    }

    void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {
        return accelerator->intersect(r, intersections, n_intersections);
    }

    AABB buildSurroundingBox(){
        //In building the bounding box we consider all the points without
        //checking if they are connected to triangles or not
        AABB obj_bbox(data->vertices);

        //Add some thickness to the box in case the triangles lie on the same plane
        //the box would be with 0 thickness and the ray would never hit it
        return AABB(obj_bbox);
    }
    AABB getSurroundingBox() const override {
        return box;
    }

protected:
    std::shared_ptr<TriangleMeshData> data;

private:
    std::shared_ptr<AccelerationStructure> accelerator;
    AABB box;
};
