
#pragma once
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <cstdlib>
#include "glm/glm.hpp"
#include "Geom/Ray.h"
#include "Materials/Material.h"
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

inline void get_sphere_uv(const vec3& p, double& u, double& v) {
    auto phi = atan2(p.z, p.x);
    auto theta = asin(p.y);
    u = 1-(phi + pi) / (2*pi);
    v = (theta + pi/2) / pi;
}

struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    double distance;
    bool front_face;
    shared_ptr<Material> mat_ptr;
    double u;
    double v;
    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal){
        front_face = (glm::dot(r.getDirection(), outward_normal) < 0);
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Common Headers

#include "Geom/Ray.h"
#endif