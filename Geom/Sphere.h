//
// Created by kevin on 13/08/20.
//
#pragma once
#include "Ray.h"
#include "../consts.h"
#include "AABB.h"
#include "../SceneGraph/Transform.h"
#include "../BVH/AABB.h"


class Sphere {
public:
    Sphere() {}
    Sphere(glm::vec3 cen, double r, shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};

    bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
        glm::vec3 oc = r.getOrigin() - center;
        auto a = glm::length(r.getDirection())*glm::length(r.getDirection());
        auto half_b = dot(oc, r.getDirection());
        auto c = glm::length(oc)*glm::length(oc) - radius*radius;
        auto discriminant = half_b*half_b - a*c;

        if (discriminant > 0) {
            auto root = sqrt(discriminant);

            auto temp = (-half_b - root) / a;
            if (temp < t_max && temp > t_min) {
                rec.distance = temp;
                rec.p = r.at(rec.distance);
                glm::vec3 outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return true;
            }

            temp = (-half_b + root) / a;
            if (temp < t_max && temp > t_min) {
                rec.distance = temp;
                rec.p = r.at(rec.distance);
                glm::vec3 outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return true;
            }
        }

        return false;
    }

    bool bounding_box(double t0, double t1, AABB& output_box) const {
        output_box = AABB(
                center - vec3(radius, radius, radius),
                center + vec3(radius, radius, radius)
                );

        return true;
    }


public:
    glm::vec3 center;
    float radius;
    shared_ptr<Material> mat_ptr;
};