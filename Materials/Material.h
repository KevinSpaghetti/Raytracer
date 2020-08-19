//
// Created by kevin on 13/08/20.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include <glm/glm.hpp>
#include "../Geom/Ray.h"

struct hit_record;
using glm::vec4;
using glm::vec3;

class Material {
public:
    virtual vec4 emitted(double u, double v, const vec3& p) const {
        return vec4(0, 0, 0, 0);
    }
    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const = 0;
};


#endif //RAYTRACER_MATERIAL_H
