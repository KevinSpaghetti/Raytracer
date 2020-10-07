//
// Created by kevin on 13/08/20.
//

#pragma once

#include "Material.h"
#include "../Geom/Ray.h"
#include "../Utils/Random.h"
#include "../Geom/ONB.h"

class Lambertian : public Material {
public:
    Lambertian(const Color a) : albedo(a) {}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        float lambert_correction_factor = glm::abs(glm::dot(wi.getDirection(), glm::normalize(i.ws_normal)));
        return albedo * lambert_correction_factor;
    }

    bool scatters(const Intersection &i, const Ray& wo) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray& wo) const override {
        Point direction = randomized::vector::in_unit_sphere();
        return Ray(i.ws_point, direction, Ray::Type::Diffuse);
    }

    bool emits(const Intersection& i, const Ray& wo) const override {
        return false;
    }
    Color emit(const Intersection& i, const Ray& wo) const override {
        return {0.0, 0.0, 0.0};
    }

private:
    Color albedo;
};


