//
// Created by kevin on 31/08/20.
//

#pragma once

#include "Material.h"

class TXTMaterial : public Material {

    Color scatter(const Ray& r, const Intersection& i) const {
        std::shared_ptr<Texture> txt = textures.at("albedo");
        return txt->value(i.uv);
    }

public:
    bool scatter(const Intersection &i, Ray &r) const override {
        return false;
    }

protected:
    Color blend(const Color &emitted, const Color &incoming) const override {
        std::shared_ptr<Texture> txt = textures.at("albedo");
        return emitted + 0.5f * incoming;
    }
};