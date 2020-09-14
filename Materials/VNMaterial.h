//
// Created by kevin on 26/08/20.
//

#pragma once

#include "Material.h"
#include "../Geom/Ray.h"
#include "../consts.h"

class VNMaterial : public Material {
public:
    bool scatter(const Intersection &i, Ray &r) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return glm::clamp((i.pv + Point(1.0f)) / 2.0f, Point(0), Point(1.0f));
    }


};