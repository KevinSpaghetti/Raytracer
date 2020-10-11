//
// Created by kevin on 04/09/20.
//

#pragma once

#include <list>
#include "../Geom/Intersection.h"

//This class is for pure geometric intersection tests, no materials
//The intersection test must respect the ray min and max bounds
class IntersectTestable {
public:
    virtual void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const = 0;
};