//
// Created by kevin on 29/09/20.
//

#pragma once

#include "Material.h"

class SolidColorMaterial : public Material {
public:
    SolidColorMaterial(Color c) : c(c) {}

    bool scatter(const Intersection &i, const Ray &incoming, Ray &outgoing) const override {
        return false;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return c;
    }

private:
    Color c;
};
