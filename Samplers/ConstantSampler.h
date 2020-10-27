//
// Created by kevin on 15/10/20.
//

#pragma once


#include "Sampler.h"
#include "../Utils/Random.h"

class ConstantSampler : public Sampler {
public:
    ConstantSampler(Point2D static_sampling_point) : static_sampling_point(static_sampling_point){}

    Point2D generateSample(int sample_n) const override {
        return static_sampling_point;
    }

private:
    Point2D static_sampling_point;

};