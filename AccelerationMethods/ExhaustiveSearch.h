//
// Created by Kevin on 12/09/2020.
//

#pragma once

#include "AccelerationStructure.h"
#include "../SceneGraph/IntersectTestable.h"
#include "../Utils/Triangle.h"

class ExhaustiveSearch : public AccelerationStructure {
public:

    ExhaustiveSearch() = delete;
    ExhaustiveSearch(std::shared_ptr<TriangleMeshData> data) :
        data(data) {}

    std::list<Intersection> intersect(const Ray& r) const override {
        std::list<Intersection> ins;

        for (Triangle tri : data->triangles) {
            float d, u, v;
            if(data->test(r, tri, d, u, v)){
                ins.emplace_back(data->interpolate(tri, u, v));
            }
        }

        return ins;
    }

private:
    std::shared_ptr<TriangleMeshData> data;

};