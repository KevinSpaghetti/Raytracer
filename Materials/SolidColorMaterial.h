//
// Created by kevin on 29/09/20.
//

#pragma once

#include "Material.h"

class SolidColorMaterial : public Material {
public:
    SolidColorMaterial(Color c) : c(c) {}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return Color{0.0, 0.0, 0.0};
    }

    bool emits(const Intersection& i, const Ray& incoming) const override {
        return true;
    }
    Color emit(const Intersection& i, const Ray& incoming) const override {
        return c;
    }

    bool scatters(const Intersection &i, const Ray &incoming) const override {
        return false;
    }
    Ray scatter(const Intersection &i, const Ray &incoming) const override {
        return Ray();
    }

private:
    Color c;
};
