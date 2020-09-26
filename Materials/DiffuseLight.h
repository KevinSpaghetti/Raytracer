//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Material.h"

class DiffuseLight : public Material {
public:
    DiffuseLight(const Color color, const float intensity) : c(color), intensity(intensity){}

    bool scatter(const Intersection &i, const Ray &incoming, Ray &outgoing) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return intensity * c;
    }

private:
    Color c;
    float intensity;

};
