//
// Created by kevin on 13/08/20.
//

#pragma once

#include "Material.h"
#include "../consts.h"
#include "../Geom/Ray.h"
#include "../Utils/Random.h"

class Lambertian : public Material {
public:
    Lambertian(const Color a) : albedo(a) {}

    bool scatter(const Intersection &i, Ray& r) const override {
        Point direction = glm::normalize(i.pn + randomized::vector::random(-1.0f, 1.0f));
        r =  Ray(i.pv, direction);
        return true;
    }

protected:

    Color blend(const Color& emitted, const Color& incoming) const override {
        return emitted + albedo * incoming;
    }

private:
    Color albedo;
};

/*
class Metal : public Material {
public:
    Metal(const glm::vec4& a) : albedo(a) {}

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        vec3 reflected = glm::reflect(glm::normalize(r_in.getDirection()), rec.normal);
        scattered = Ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.getDirection(), rec.normal) > 0);
    }

private:
    glm::vec4 albedo;
};
*/

