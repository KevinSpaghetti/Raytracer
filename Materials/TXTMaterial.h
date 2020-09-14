//
// Created by kevin on 31/08/20.
//

#pragma once

#include "Material.h"

class TXTMaterial : public Material {
public:
    bool scatter(const Intersection &i, Ray &r) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        std::shared_ptr<Texture> txt = textures.at("albedo");
        return 0.5f * incoming;
    }
};