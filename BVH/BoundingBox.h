//
// Created by kevin on 04/09/20.
//

#pragma once

#include "../SceneGraph/Hittable.h"
#include "CanQueryForHits.h"

//Since it is a box we can always ask for the max and min points
class BoundingBox : public CanQueryForHits {
public:
    virtual Point getMin() const = 0;
    virtual Point getMax() const = 0;

};
