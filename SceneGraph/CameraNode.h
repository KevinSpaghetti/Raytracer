//
// Created by kevin on 15/10/20.
//

#pragma once

#include "Node.h"

class CameraNode : public Node {
public:
    CameraNode(Point lookfrom, Point lookat, Normal up, float vfov, float aspect_ratio, float aperture, float focus_dist) {
        auto theta = glm::radians(vfov);
        auto h = tanf(theta/2.0f);
        float viewport_height = 10.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(up, w));
        v = glm::cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        upper_left_corner = origin - horizontal/2.0f + vertical/2.0f - focus_dist * w;

        lens_radius = aperture / 2.0f;
    }

    Ray get(float s, float t) const {
        Point rd = lens_radius * randomized::vector::in_unit_disk();
        Point offset = u * rd.x + v * rd.y;

        const Point r_origin = origin + offset;
        const Point r_direction = upper_left_corner + s*horizontal - t*vertical - origin - offset;
        return Ray(global.pointToWorldSpace(r_origin),
                   global.directionToWorldSpace(r_direction),
                   Ray::Type::Camera);
    }

    Type type() const override {
        return Type::Camera;
    }

private:
    Point origin{};
    Point upper_left_corner{};
    Point horizontal{};
    Point vertical{};
    Point u{}, v{}, w{};
    float lens_radius{};
};
