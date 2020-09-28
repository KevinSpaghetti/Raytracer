//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include "../Utils/Structs.h"
#include "../Geom/Ray.h"

class Integrator {
public:

    virtual Color sample(const Ray& r) const = 0;


};
