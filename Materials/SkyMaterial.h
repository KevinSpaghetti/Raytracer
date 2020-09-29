//
// Created by kevin on 29/09/20.
//

#pragma once

#include "Material.h"

class SkyMaterial : public Material {
public:
    bool scatter(const Intersection &i, const Ray &incoming, Ray &outgoing) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        Point unit_direction = r.getDirection();
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * Color(1.0) + t * Color{0.5, 0.7, 1.0};
    }

};
