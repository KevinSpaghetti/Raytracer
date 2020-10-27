//
// Created by kevin on 15/10/20.
//

#pragma once

#include "Sampler.h"
#include "../Utils/Random.h"

class StratifiedUniformSampler : public Sampler {
public:
    StratifiedUniformSampler(int nOfSamples) : n_of_total_samples(nOfSamples), n_of_axis_stratas(sqrt(nOfSamples)) {}

    Point2D generateSample(int sample_n) const override {
        Point2D strata_dimensions = 1.0f / Point2D{n_of_axis_stratas, n_of_axis_stratas};
        //find the strata center for the n sample
        int sample_strata_row = floor(sample_n / n_of_axis_stratas);
        int sample_strata_column = sample_n - (sample_strata_row * n_of_axis_stratas);
        return glm::mod((strata_dimensions * Point2D{sample_strata_column, sample_strata_row}) + (strata_dimensions/2.0f),1.0f);
    }

protected:
    int n_of_total_samples;
    int n_of_axis_stratas; //number of grid rectangles for axis
};
class StratifiedJitteredSampler : public StratifiedUniformSampler {
public:
    using StratifiedUniformSampler::StratifiedUniformSampler;

    Point2D generateSample(int sample_n) const override {
        Point2D strata_dimensions = 1.0f / Point2D{n_of_axis_stratas, n_of_axis_stratas};
        Point2D uniform_point = StratifiedUniformSampler::generateSample(sample_n);
        Point2D random_jitter = {randomized::scalar::random(-0.5, 0.5), randomized::scalar::random(-0.5, 0.5)};
        return uniform_point + (random_jitter * strata_dimensions);
    }

};