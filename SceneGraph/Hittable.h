//
// Created by kevin on 24/08/20.
//

#pragma once

#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"

class Hittable {
public:
    virtual std::list<ObjectIntersection> hit(const Ray& r) = 0;


};
