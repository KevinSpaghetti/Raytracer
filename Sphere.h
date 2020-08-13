//
// Created by kevin on 13/08/20.
//
#include "Ray.h"
#include "consts.h"


class Hittable {
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, hit_record& rec) const = 0;
};

class Sphere : public Hittable {
public:
    Sphere() {}
    Sphere(glm::vec3 cen, double r, shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
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
                rec.mat_ptr = mat_ptr;
                return true;
            }

            temp = (-half_b + root) / a;
            if (temp < t_max && temp > t_min) {
                rec.distance = temp;
                rec.p = r.at(rec.distance);
                glm::vec3 outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                rec.mat_ptr = mat_ptr;
                return true;
            }
        }

        return false;
    }

public:
    glm::vec3 center;
    float radius;
    shared_ptr<Material> mat_ptr;
};