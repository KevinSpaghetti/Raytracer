//
// Created by kevin on 04/10/20.
//

#pragma once

#include "../Utils/Structs.h"

class ImportanceSamplingEnabled {
    //Importance sampling points and directions are all passed in world space

    //
    virtual double pdf(const Point& origin, const Normal& direction) const = 0;

    //Return a random point on the light surface
    virtual Point random(const Point& o) const = 0;
};

