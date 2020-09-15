//
// Created by kevin on 31/08/20.
//

#pragma once

#include "Material.h"

class TXTMaterial : public Material {
public:
    bool scatter(const Intersection &i, Ray &r) const override {
        Point direction = glm::normalize(randomized::vector::random(-1.0f, 1.0f));
        r =  Ray(i.pv, direction);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        std::shared_ptr<Texture> txt = textures.at("albedo");
        return 0.8f * txt->value(i.uv) + 0.2f * incoming;
    }
};