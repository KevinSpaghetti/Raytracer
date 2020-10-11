//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Mesh.h"

class SphereMesh : public Mesh {
public:

    SphereMesh(glm::vec3 center, float radius) : center(center), radius(radius) {}

    void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {

        glm::vec3 oc = r.getOrigin() - center;
        auto a = pow(glm::length(r.getDirection()), 2.0f);
        auto half_b = glm::dot(oc, r.getDirection());
        auto c = pow(glm::length(oc),2.0f) - pow(radius, 2.0f);
        auto discriminant = half_b*half_b - a*c;

        n_intersections = 0;
        if (discriminant > 0) {
            float root = sqrtf(discriminant);
            float dist = (-half_b - root) / a;

            if (dist < r.getTmax() && dist > r.getTmin()) {
                Point ip = r.at(dist);
                Point n = (ip - center) / radius;
                float u = atan2f(n.x, n.z) / (2*consts::pi) + 0.5f;
                float v = n.y * 0.5f + 0.5f;
                bool isFront;
                if(glm::dot(r.getDirection(), n) < 0){
                    isFront = true;
                }else{
                    isFront = false;
                    n = -n;
                };
                intersections[n_intersections] = {
                    ip,
                    n,
                    dist,
                    //Invert the y axis
                    UV{u, 1.0f - v, 0},
                    isFront
                };
                ++n_intersections;
            }
            dist = (-half_b + root) / a;
            if (dist < r.getTmax() && dist > r.getTmin()) {
                Point ip = r.at(dist);
                Point n = (ip - center) / radius;
                float u = atan2f(n.x, n.z) / (2*consts::pi) + 0.5f;
                float v = n.y * 0.5f + 0.5f;
                bool isFront;
                if(glm::dot(r.getDirection(), n) < 0){
                    isFront = true;
                }else{
                    isFront = false;
                    n = -n;
                };
                intersections[n_intersections] = {
                        ip,
                        n,
                        dist,
                        UV{u, 1.0f - v, 0},
                        isFront
                };
                ++n_intersections;
            }
        }
    }

    AABB getSurroundingBox() const override {
        return AABB(center - radius, center + radius);
    }

private:
    glm::vec3 center;
    float radius;

};