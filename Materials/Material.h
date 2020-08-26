//
// Created by kevin on 13/08/20.
//
#pragma once
#include <glm/glm.hpp>
#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"


class Material {
public:
    virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) const {
        return glm::vec3(0, 0, 0);
    }
    virtual glm::vec3 scatter(const Ray& r, const Intersection& i) const = 0;

};
