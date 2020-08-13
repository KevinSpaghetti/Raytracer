//
// Created by kevin on 13/08/20.
//
#include "Ray.h"

struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    double distance;
};

class Hittable {
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, hit_record& rec) const = 0;
};

class sphere : public Hittable {
public:
    sphere() {}
    sphere(glm::vec3 cen, double r) : center(cen), radius(r) {};

    virtual bool hit(const Ray& r, double tmin, double tmax, hit_record& rec) const override;

public:
    glm::vec3 center;
    float radius;
};

bool sphere::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
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
            rec.normal = (rec.p - center) / radius;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.distance = temp;
            rec.p = r.at(rec.distance);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
    }

    return false;
}