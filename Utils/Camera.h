#pragma once

#include "../consts.h"
#include "Random.h"

using glm::vec3;

class Camera {
public:
    Camera(){}

    Camera(vec3 lookfrom, vec3 lookat, vec3 up, float vfov, float aspect_ratio, float aperture, float focus_dist) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        float viewport_height = 10.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(up, w));
        v = glm::cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        upper_left_corner = origin - horizontal/2.0f + vertical/2.0f - focus_dist * w;

        lens_radius = aperture / 2;
    }

    Ray get(float s, float t) const {
        //vec3 rd = lens_radius * randomized::vector::in_unit_disk();
        vec3 offset = {0, 0, 0}; //u * rd.x + v * rd.y;
        return Ray(origin + offset, upper_left_corner + s*horizontal - t*vertical - origin - offset);
    }

private:
    vec3 origin{};
    vec3 upper_left_corner{};
    vec3 horizontal{};
    vec3 vertical{};
    vec3 u{}, v{}, w{};
    float lens_radius{};
};