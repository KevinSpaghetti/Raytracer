//
// Created by Kevin on 14/09/2020.
//

#pragma once

#include "Material.h"

class Metal : public Material {
public:
    Metal(const Color a) : albedo(a) {}

    bool scatter(const Intersection &i, Ray& r) const override {
        Point v = r.getDirection();
        Point n = i.pn;
        Point reflection = v - 2*glm::dot(v,n)*n;
        r =  Ray(i.pv, reflection);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return albedo * incoming;
    }

private:
    Color albedo;
};