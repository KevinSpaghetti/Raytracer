//
// Created by Kevin on 12/09/2020.
//

#pragma once

#include "AccelerationStructure.h"
#include "../Geom/IntersectTestable.h"
#include "../Utils/Triangle.h"

class ExhaustiveSearch : public AccelerationStructure {
public:

    ExhaustiveSearch() = delete;
    ExhaustiveSearch(std::shared_ptr<TriangleMeshData> data) :
        data(std::move(data)) {}

    void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {
        Intersection closest{r.at(consts::infinity)};
        std::for_each(data->triangles.begin(), data->triangles.end(), [&](Triangle t){
            Intersection i{};
            if(data->test(r, t, i)){
                if(glm::length(i.point - r.getOrigin()) < glm::length(closest.point - r.getOrigin())){
                    closest = i;
                    n_intersections = 1;
                }
            }
        });
        if(n_intersections > 0){
            n_intersections = 1;
            intersections[0] = closest;
        }
    }

private:
    std::shared_ptr<TriangleMeshData> data;

};