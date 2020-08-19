//
// Created by kevin on 15/08/20.
//

#ifndef RAYTRACER_RECT_H
#define RAYTRACER_RECT_H


#include "../Materials/Material.h"
#include "../Hittable_List.h"

class Rect : public Hittable {
public:
    Rect() {}
    Rect(double _x0, double _x1, double _y0, double _y1, double _k,
         shared_ptr<Material> mat): x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

    bool hit(const Ray& r, double t0, double t1, hit_record& rec) const {
        auto t = (k-r.getOrigin().z) / r.getDirection().z;
        if (t < t0 || t > t1)
            return false;
        auto x = r.getOrigin().x + t*r.getDirection().x;
        auto y = r.getOrigin().y + t*r.getDirection().y;
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;
        rec.u = (x-x0)/(x1-x0);
        rec.v = (y-y0)/(y1-y0);
        rec.distance = t;
        auto outward_normal = vec3(0, 0, 1);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mp;
        rec.p = r.at(t);
        return true;
    }

    bool bounding_box(double t0, double t1, AABB& output_box) const {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = AABB(vec3(x0,y0, k-0.0001), vec3(x1, y1, k+0.0001));
        return true;
    }
public:

    shared_ptr<Material> mp;
    double x0, x1, y0, y1, k;
};


#endif //RAYTRACER_RECT_H
