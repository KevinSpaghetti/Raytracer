//
// Created by kevin on 24/08/20.
//

#pragma once

#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//Check if the ray hits the object, with which material, done for
//logic intersection with an object
class Hittable {
public:
    virtual void hit(const Ray& r, std::list<ObjectIntersection>& intersections) = 0;


};
