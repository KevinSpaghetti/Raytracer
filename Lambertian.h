//
// Created by kevin on 13/08/20.
//

#ifndef RAYTRACER_LAMBERTIAN_H
#define RAYTRACER_LAMBERTIAN_H

#include "Material/Material.h"
#include "consts.h"
#include "Geom/Ray.h"
#include "main.cpp"

class Lambertian : public Material {
public:
    Lambertian(const glm::vec4& a) : albedo(a) {}

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    glm::vec4 albedo;
};

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

#endif //RAYTRACER_LAMBERTIAN_H
