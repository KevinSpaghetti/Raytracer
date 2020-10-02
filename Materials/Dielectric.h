//
// Created by kevin on 28/09/20.
//

#pragma once

#include "Material.h"
#include "../Utils/Random.h"

class Dielectric : public Material{
public:
    Dielectric(const float index) : index(index) {}

    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        float etai;
        if(i.isFront) {
            etai = 1.0f / index;
        }else {
            etai = index;
        }

        float cos_theta = fminf(glm::dot(-incoming.getDirection(), i.pn), 1.0);
        float sin_theta = sqrtf(1.0f - powf(cos_theta, 2));

        if (etai * sin_theta > 1.0) {
            Point reflected = glm::reflect(incoming.getDirection(), i.pn);
            outgoing = Ray(i.pv, reflected);
            return true;
        }
        double reflect_prob = schlick(cos_theta, etai);
        if (randomized::scalar::random() < reflect_prob){
            Point reflected = glm::reflect(incoming.getDirection(), glm::normalize(i.pn));
            outgoing = Ray(i.pv, reflected);
            return true;
        }

        Point refracted = glm::refract(incoming.getDirection(), i.pn ,etai);
        outgoing = Ray(i.pv, refracted);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return incoming;
    }

private:
    float schlick(float cosine, float ref_idx) const {
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }

private:
    float index;
};
