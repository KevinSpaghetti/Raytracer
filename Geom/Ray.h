//
// Created by kevin on 12/08/20.
//
#pragma once

#include <glm/glm.hpp>

class Ray {
public:
    Ray() {}
    Ray(const glm::vec3 origin, const glm::vec3 direction) : origin(origin), direction(direction) {}

    glm::vec3 getOrigin() const { return origin; }
    glm::vec3 getDirection() const { return direction; }
    glm::vec3 at(float distance) const { return origin + (direction * distance); }

private:
    glm::vec3 origin;
    glm::vec3 direction;

};
