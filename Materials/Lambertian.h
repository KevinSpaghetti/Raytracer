//
// Created by kevin on 13/08/20.
//

#pragma once

#include "Material.h"
#include "../Geom/Ray.h"
#include "../Utils/Random.h"

class Lambertian : public Material {
public:
    Lambertian(const Color a) : albedo(a) {}

    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        Point direction = i.pn + randomized::vector::unit_vector();
        outgoing =  Ray(i.pv, direction);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return albedo * incoming;
    }

private:
    Color albedo;
};


