//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Mesh.h"

class SphereMesh : public Mesh {
public:

    SphereMesh(glm::vec3 center, float radius) : center(center), radius(radius) {}

    std::list<Intersection> intersect(const Ray& r) const override {
        std::list<Intersection> intersections;

        glm::vec3 oc = r.getOrigin() - center;
        auto a = pow(glm::length(r.getDirection()), 2);
        auto half_b = glm::dot(oc, r.getDirection());
        auto c = pow(glm::length(oc),2) - pow(radius, 2);
        auto discriminant = half_b*half_b - a*c;

        if (discriminant > 0) {
            auto root = sqrt(discriminant);
            auto temp = (-half_b - root) / a;

            if (temp < r.getTmax() && temp > r.getTmin()) {
                Point ip = r.at(temp);
                float phi = atan2(ip.z, ip.x);
                float theta = asin(ip.y);
                float u = 1-(phi + pi) / (2*pi);
                float v = (theta + pi/2) / pi;
                intersections.push_back(Intersection{
                    ip,
                    (ip - center) / radius,
                    UV{u, v, 0}
                });
            }
            temp = (-half_b + root) / a;
            if (temp < r.getTmax() && temp > r.getTmin()) {
                Point ip = r.at(temp);
                float phi = atan2(ip.z, ip.x);
                float theta = asin(ip.y);
                float u = 1-(phi + pi) / (2*pi);
                float v = (theta + pi/2) / pi;
                intersections.push_back(Intersection{
                        ip,
                        (ip - center) / radius,
                        UV{u, v, 0}
                });
            }
        }
        return intersections;
    }

    AABB getSurroundingBox() const override {
        return AABB(center - radius, center + radius);
    }

private:
    glm::vec3 center;
    float radius;

};