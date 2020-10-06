//
// Created by kevin on 04/10/20.
//

#pragma once

#include "../Utils/Structs.h"
#include "../Utils/PDF.h"

class ImportanceSamplingEnabled {
public:
    //Importance sampling points and directions are all passed in world space

    //Return a ray from origin towards a random point on the light in world space
    virtual Color random(const Point& origin) const = 0;


};

