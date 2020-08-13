
#pragma once
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <cstdlib>
#include "glm/glm.hpp"
#include "Ray.h"
#include "Material.h"
// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using glm::vec3;
// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline vec3 random(double min, double max) {
    return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}

struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    double distance;
    bool front_face;
    shared_ptr<Material> mat_ptr;

    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal){
        front_face = (glm::dot(r.getDirection(), outward_normal) < 0);
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Common Headers

#include "Ray.h"
#endif