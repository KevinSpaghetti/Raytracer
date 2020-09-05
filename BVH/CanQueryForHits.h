//
// Created by kevin on 04/09/20.
//

#pragma once

#include "../Geom/Ray.h"

class CanQueryForHits {
public:
    virtual bool isHit(const Ray& r) = 0;
};