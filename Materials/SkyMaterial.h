//
// Created by kevin on 29/09/20.
//

#pragma once

#include "Material.h"

class SkyMaterial : public Material {
public:

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return Color{0.0, 0.0, 0.0};
    }

    bool scatters(const Intersection &i, const Ray &incoming) const override {
        return false;
    }
    Ray scatter(const Intersection &i, const Ray &incoming) const override {
        return Ray();
    }

    bool emits(const Intersection& i, const Ray& incoming) const override {
        return true;
    }
    Color emit(const Intersection& i, const Ray& incoming) const override {
        Point unit_direction = incoming.getDirection();
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * Color(1.0) + t * Color{0.5, 0.7, 1.0};
    }

};
