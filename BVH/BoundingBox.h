//
// Created by Kevin on 12/09/2020.
//

#pragma once

#include "../Utils/Structs.h"
#include "CanQueryForHits.h"

class BoundingBox : public CanQueryForHits {
public:

    virtual Point getMin() const = 0;
    virtual Point getMax() const = 0;

};