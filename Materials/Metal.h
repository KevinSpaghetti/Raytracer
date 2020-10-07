//
// Created by Kevin on 14/09/2020.
//

#pragma once

#include "Material.h"

class Metal : public Material {
public:
    Metal(const Color a, const float fuzziness) : albedo(a), fuzziness(fuzziness) {}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return albedo;
    }

    bool scatters(const Intersection &i, const Ray &wo) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray &wo) const override {
        Normal unit_normal = glm::normalize(i.ws_normal);
        Point reflected = glm::reflect(-wo.getDirection(), unit_normal + fuzziness * randomized::vector::in_unit_sphere());
        return Ray(i.ws_point, reflected, Ray::Type::Specular);
    }

    bool emits(const Intersection& i, const Ray& wo) const override {
        return false;
    }
    Color emit(const Intersection& i, const Ray& wo) const override {
        return {0.0, 0.0, 0.0};
    }

private:
    Color albedo;
    float fuzziness;
};