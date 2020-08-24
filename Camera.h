#ifndef CAMERA_H
#define CAMERA_H

#include "consts.h"

using glm::vec3;

vec3 random_unit_vector() {
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return vec3(r*cos(a), r*sin(a), z);
}
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1,1);
        if (glm::length(p)*glm::length(p) >= 1) continue;
        return p;
    }
}
vec3 random_in_unit_disk(){
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (glm::length(p)*glm::length(p) >= 1) continue;
        return p;
    }
}

class Camera {
public:
    Camera(vec3 lookfrom, vec3 lookat, vec3 up, float vfov, float aspect_ratio, float aperture, float focus_dist) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        float viewport_height = 2.0 * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(up, w));
        v = glm::cross(w, u);
        auto focal_length = 1.0;

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal/2.0f - vertical/2.0f - focus_dist * w;

        lens_radius = aperture / 2;
    }

    Ray get_ray(float s, float t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x + v * rd.y;
        return Ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
    }

private:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;
};
#endif