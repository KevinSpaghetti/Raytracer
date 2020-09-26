//
// Created by kevin on 04/09/20.
//

#pragma once

#include <list>
#include "../Geom/Intersection.h"

//This class is for pure geometric intersection tests, no materials
//The intersection test must respect the ray min and max bounds
//TODO: change to support choosing between backface culling options
class IntersectTestable {
public:
    virtual std::vector<Intersection> intersect(const Ray& r) const = 0;
};