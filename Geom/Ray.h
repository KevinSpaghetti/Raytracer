//
// Created by kevin on 12/08/20.
//
#pragma once

#include <glm/glm.hpp>
#include "../Utils/Consts.h"

class Ray {
public:
    Ray() {}
    Ray(const glm::vec3 origin, const glm::vec3 direction, const float tmin = 0.001, const float tmax = consts::infinity) : origin(origin), direction(direction), tmin(tmin), tmax(tmax) {}

    glm::vec3 getOrigin() const { return origin; }
    glm::vec3 getDirection() const { return direction; }
    glm::vec3 at(float distance) const { return origin + (direction * distance); }

    float getTmin() const {
        return tmin;
    }
    float getTmax() const {
        return tmax;
    }

private:
    glm::vec3 origin;
    glm::vec3 direction;

    float tmin;
    float tmax;
};
