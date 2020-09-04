//
// Created by kevin on 27/08/20.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"
#include "../Geom/Intersection.h"
#include "IntersectTestable.h"

//Represents an empty mesh
class Mesh : public IntersectTestable {
public:
    Mesh() {}

    std::list<Intersection> intersect(const Ray &r) const override {
        return std::list<Intersection>();
    }

};

