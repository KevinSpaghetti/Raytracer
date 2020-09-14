//
// Created by kevin on 24/08/20.
//

#pragma once

#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//Check if the ray hits the object, with which material, done for
//logic intersection with an object
//The function Hit must overwrite the ray if trasformation occurs
class Hittable {
public:
    virtual std::list<ObjectIntersection> hit(Ray& r) = 0;


};
