//
// Created by kevin on 26/08/20.
//

#pragma once

#include "Material.h"
#include "../Geom/Ray.h"
#include "../consts.h"

class VNMaterial : public Material {
public:
    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return (i.pn + Point(1.0f)) * 0.5f;
    }


};