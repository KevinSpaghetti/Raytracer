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
                intersections.push_back(Intersection{
                    Vertex{r.at(temp)},
                    (r.at(temp) - center) / radius,
                    UV{0, 0, 0}
                });
            }
            temp = (-half_b + root) / a;
            if (temp < r.getTmax() && temp > r.getTmin()) {
                intersections.push_back(Intersection{
                        Vertex{r.at(temp)},
                        (r.at(temp) - center) / radius,
                        UV{0, 0, 0}
                });
            }
        }
        return intersections;
    }

private:
    glm::vec3 center;
    float radius;

};