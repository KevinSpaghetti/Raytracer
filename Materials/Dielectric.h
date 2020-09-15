//
// Created by Kevin on 15/09/2020.
//

#pragma once

#include "Material.h"

class Dielectric : public Material {
public:
    Dielectric(float ior) : ior(ior) {}

    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        Point refracted = glm::refract(glm::normalize(incoming.getDirection()), glm::normalize(i.pn), ior);
        outgoing =  Ray(i.pv, refracted);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return  incoming;
    }

private:
    float ior;
};