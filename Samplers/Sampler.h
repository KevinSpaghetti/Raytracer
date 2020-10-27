//
// Created by kevin on 15/10/20.
//

#pragma once

#include "../Utils/Structs.h"

class Sampler {
public:
    //Generate samples in the (0, 0) - (1, 1) pixel space (from top left to bottom right)
    //sample_n is a positive sequence that will be passed when a sample is needed starting from 0
    virtual Point2D generateSample(int sample_n) const = 0;
};