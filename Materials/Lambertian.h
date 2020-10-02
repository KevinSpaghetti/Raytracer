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

    bool scatter(const Intersection &i, const Ray& incoming, Ray& outgoing) const override {
        //ONB uvw = ONB::build_from_w(i.pn);
        //Point direction = uvw.local(randomized::vector::in_cosine_direction());

        Point direction = randomized::vector::in_unit_sphere();
        outgoing =  Ray(i.pv, direction);
        return true;
    }

    Color color(const Intersection &i, const Ray &r, const Color &incoming) const override {
        return albedo * incoming;
    }
    Color color(const Intersection &i, const Ray &r, const Color &incoming, const float pdf) const override {
        return albedo * pdf * incoming / pdf;
    }

    float sPDF(const Intersection &i, const Ray &r, const Ray &outgoing) const override {
        float cosine = glm::dot(i.pn, outgoing.getDirection());
        return glm::dot(i.pn, outgoing.getDirection()) / consts::pi;
    }


private:
    Color albedo;
};


