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

    std::vector<Intersection> intersect(const Ray& r) const override {
        std::vector<Intersection> ins;

        std::for_each(data->triangles.begin(), data->triangles.end(), [&](Triangle t){
            Intersection i{};
            if(data->test(r, t, i)){
                ins.emplace_back(i);
            }
        });

        return ins;
    }

private:
    std::shared_ptr<TriangleMeshData> data;

};