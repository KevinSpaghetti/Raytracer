//
// Created by kevin on 31/08/20.
//

#pragma once

#include "Material.h"

class TXTMaterial : public Material {
    Color scatter(const Ray& r, const Intersection& i) const {
        std::shared_ptr<Texture> txt = textures.at("albedo");
        return {0.3, 0.3, 0.3};
    }
};