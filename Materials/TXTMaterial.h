//
// Created by kevin on 31/08/20.
//

#pragma once

#include "Material.h"

class TXTMaterial : public Material {
public:

    virtual Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return textures.at("albedo")->value(i.uv) / consts::pi;
    }

    bool scatters(const Intersection &i, const Ray &incoming) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray& incoming) const override {
        Point direction = randomized::vector::in_unit_hemisphere(i.ws_normal);
        return Ray(i.ws_point, direction, Ray::Type::Diffuse);
    }

    virtual bool emits(const Intersection& i, const Ray& incoming) const override {
        return false;
    }
    virtual Color emit(const Intersection& i, const Ray& incoming) const override {
        return {0.0, 0.0, 0.0};
    }

};