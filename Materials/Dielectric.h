//
// Created by kevin on 28/09/20.
//

#pragma once

#include "Material.h"
#include "../Utils/Random.h"

class Dielectric : public Material{
public:
    Dielectric(const float index) : index(index) {}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return Color{1.0, 1.0, 1.0};
    }

    bool scatters(const Intersection &i, const Ray &wo) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray& wo) const override {
        float etai;
        if(i.isFront) {
            etai = 1.0f / index;
        }else {
            etai = index;
        }

        Normal unit_normal = glm::normalize(i.ws_normal);
        Normal unit_direction = glm::normalize(-wo.getDirection());

        float cos_theta = fminf(glm::dot(-unit_direction, unit_normal), 1.0);
        float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);

        bool cannot_refract = etai * sin_theta > 1.0f;
        Normal direction;
        if (cannot_refract || schlick(cos_theta, etai) > randomized::scalar::random(0.0f, 1.0f)) {
            direction = glm::reflect(unit_direction, unit_normal);
        }else{
            direction = glm::refract(unit_direction, unit_normal, etai);
        }
        return Ray(i.ws_point, direction, Ray::Type::Transmission);
    }

    bool emits(const Intersection& i, const Ray& wo) const override {
        return false;
    }
    Color emit(const Intersection& i, const Ray& wo) const override {
        return {0.0, 0.0, 0.0};
    }

private:

    static float schlick(float cosine, float ref_idx) {
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*powf((1 - cosine), 5.0f);
    }

    float index;
};
