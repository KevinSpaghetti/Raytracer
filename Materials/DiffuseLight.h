//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Material.h"

class DiffuseLight : public Material {
public:
    DiffuseLight(const Color color, const float intensity) : c(color), intensity(intensity){}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        //The light only emits, the rendering equation second term is always 0
        return {0.0, 0.0, 0.0};
    }

    bool scatters(const Intersection &i, const Ray& wo) const override {
        return false;
    }
    Ray scatter(const Intersection &i, const Ray& wo) const override {
        return Ray();
    }

    bool emits(const Intersection& i, const Ray& wo) const override {
        return true;
    };
    //We can emit based on the distance from the ray origin
    Color emit(const Intersection& i, const Ray& wo) const override {
        return c * intensity;
    };

private:
    Color c;
    float intensity;
};
