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

    bool scatters(const Intersection &i, const Ray &incoming) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray& incoming) const override {
        float etai;
        if(i.isFront) {
            etai = 1.0f / index;
        }else {
            etai = index;
        }

        float cos_theta = fminf(glm::dot(-incoming.getDirection(), i.ws_normal), 1.0);
        float sin_theta = sqrtf(1.0f - powf(cos_theta, 2));

        if (etai * sin_theta > 1.0) {
            Point reflected = glm::reflect(incoming.getDirection(), i.ws_normal);
            return Ray(i.ws_point, reflected, Ray::Type::Specular);
        }
        double reflect_prob = schlick(cos_theta, etai);
        if (randomized::scalar::random() < reflect_prob){
            Point reflected = glm::reflect(incoming.getDirection(), glm::normalize(i.ws_normal));
            return Ray(i.ws_point, reflected, Ray::Type::Specular);
        }

        Point refracted = glm::refract(incoming.getDirection(), i.ws_normal , etai);
        return Ray(i.ws_point, refracted, Ray::Type::Transmission);
    }

    bool emits(const Intersection& i, const Ray& incoming) const override {
        return false;
    }
    Color emit(const Intersection& i, const Ray& incoming) const override {
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
