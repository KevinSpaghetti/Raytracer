//
// Created by kevin on 15/08/20.
//
#pragma once
#ifndef RAYTRACER_BVH_H
#define RAYTRACER_BVH_H

#include <algorithm>
#include <iostream>
#include "../consts.h"
#include "../Hittable_List.h"

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

inline bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis) {
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}

//X
inline bool box_x_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

//Y
inline bool box_y_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

//Z
inline bool box_z_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}

class BVH : public Hittable {
public:
    BVH();

    BVH(hittable_list& list, double time0, double time1) : BVH(list.objects, 0, list.objects.size(), time0, time1){}

    BVH(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, double time0, double time1){
        int axis = random_int(0, 2);
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                        : box_z_compare;

        size_t object_span = end - start;
        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2){
            if(comparator(objects[start], objects[start+1])){
                left = objects[start];
                right = objects[start+1];
            }else{
                left = objects[start];
                right = objects[start+1];
            }
        }else{
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            auto mid = start + (object_span / 2);

            left = make_shared<BVH>(objects, start, mid, time0, time1);
            right = make_shared<BVH>(objects, mid, end, time0, time1);
        }

        AABB box_left, box_right;
        if(!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
            std::cerr << "No Bounding box in bvh_node constructor\n";

        box = surrounding_box(box_left, box_right);
    }

    bool hit(const Ray& r, double tmin, double tmax, hit_record& rec) const {
        if(!box.hit(r, tmin, tmax)) return false;

        bool hit_left = left->hit(r, tmin, tmax, rec);
        bool hit_right = right->hit(r, tmin, tmax, rec);

        return hit_left || hit_right;
    }

    virtual bool bounding_box(double t0, double t1, AABB& output_box) const {
        output_box = box;
        return true;
    }

public:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB box;
};


#endif //RAYTRACER_BVH_H
