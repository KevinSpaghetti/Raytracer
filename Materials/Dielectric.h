//
// Created by kevin on 28/09/20.
//

#pragma once

#include "Material.h"
#include "../Utils/Random.h"

class Dielectric : public Material {
public:
    Dielectric(const Color albedo, const float index) : albedo(albedo), index(index) {}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        return albedo;
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

        float cos_theta = glm::clamp(glm::dot(wo.getDirection(), unit_normal), -1.0f, 1.0f);
        float sin_theta = sqrtf(std::max(0.0001f, 1.0f - cos_theta * cos_theta));

        bool cannot_refract = etai * sin_theta > 1.0f;
        Normal direction;
        if (cannot_refract || schlick(cos_theta, etai) > randomized::scalar::random(0.0f, 1.0f)) {
            direction = glm::reflect(-wo.getDirection(), unit_normal);
        }else{
            direction = refract(-wo.getDirection(), unit_normal, etai);
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
    static Point refract(const Point& uv, const Point& n, float etai_over_etat) {
        float cos_theta = dot(-uv, n);
        Point r_out_perp =  etai_over_etat * (uv + cos_theta*n);
        float lt = glm::length(r_out_perp);
        Point r_out_parallel = -sqrt(fabs(1.0f - lt*lt)) * n;
        return r_out_perp + r_out_parallel;
    }
    static float schlick(float cosine, float ref_idx) {
        auto r0 = (1.0f-ref_idx) / (1.0f+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*powf((1.0f - cosine), 5.0f);
    }

    Color albedo;
    float index;
};
