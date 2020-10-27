//
// Created by kevin on 15/10/20.
//

#pragma once

#include "Sampler.h"
#include "../Utils/Random.h"

class RandomSampler : public Sampler {
public:
    Point2D generateSample(int sample_n) const override {
        return {randomized::scalar::random(0, 1), randomized::scalar::random(0, 1)};
    }

};