//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Mesh.h"

class SphereMesh : public Mesh {
public:

    SphereMesh(glm::vec3 center, float radius) : center(center), radius(radius) {}

    void intersect(const Ray& r, std::vector<Intersection>& intersections) const override {

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
                intersections.push_back(Intersection{
                    ip,
                    n,
                    //Invert the y axis
                    UV{u, 1.0f - v, 0},
                    isFront
                });
            }
            temp = (-half_b + root) / a;
            if (temp < r.getTmax() && temp > r.getTmin()) {
                Point ip = r.at(temp);
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
                intersections.push_back(Intersection{
                        ip,
                        n,
                        UV{u, 1.0f - v, 0},
                        isFront
                });
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