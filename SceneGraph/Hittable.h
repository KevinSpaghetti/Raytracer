//
// Created by kevin on 24/08/20.
//

#pragma once

#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"

class Hittable {
public:
    virtual std::list<Intersection> hit(const Ray& r, double t_min, double t_max) = 0;


};
