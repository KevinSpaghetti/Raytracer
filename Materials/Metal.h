//
// Created by Kevin on 14/09/2020.
//

#pragma once

#include "Material.h"

class Metal : public Material {
public:
    Metal(const Color a, const float fuzziness) : albedo(a), fuzziness(fuzziness) {}

    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        Point reflected = glm::reflect(glm::normalize(incoming.getDirection()), glm::normalize(i.pn) + fuzziness * randomized::vector::in_unit_sphere());
        outgoing =  Ray(i.pv, reflected);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return albedo * incoming;
    }

private:
    Color albedo;
    float fuzziness;
};