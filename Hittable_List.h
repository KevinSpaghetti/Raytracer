#pragma once
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Geom/AABB.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class Hittable {
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double t0, double t1, AABB& output_box) const = 0;
};


class hittable_list : public Hittable {
public:
    hittable_list() {}
    hittable_list(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto& object : objects) {
            if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.distance;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
    bool bounding_box(double t0, double t1, AABB& output_box) const {
        if(objects.empty()) return false;

        AABB temp_box;
        bool first_box = true;

        for (const auto& object : objects) {
            if (!object->bounding_box(t0, t1, temp_box)) return false;
            output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
            first_box = false;
        }

        return true;

    };


public:
    std::vector<shared_ptr<Hittable>> objects;
};

#endif