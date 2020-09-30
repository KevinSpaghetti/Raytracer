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

    void intersect(const Ray& r, std::vector<Intersection>& intersections) const override {

        std::for_each(data->triangles.begin(), data->triangles.end(), [&](Triangle t){
            Intersection i{};
            if(data->test(r, t, i)){
                intersections.emplace_back(i);
            }
        });

    }

private:
    std::shared_ptr<TriangleMeshData> data;

};