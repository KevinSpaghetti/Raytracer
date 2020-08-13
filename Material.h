//
// Created by kevin on 13/08/20.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "consts.h"

struct hit_record;

class Material {
public:
    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const = 0;
};


#endif //RAYTRACER_MATERIAL_H
