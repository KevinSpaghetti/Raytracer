//
// Created by kevin on 15/08/20.
//

#pragma once

#include "../Mesh/Mesh.h"
#include "../BVH/Boxable.h"

//TODO: test uvs

//Represents an infinite plane
class PlaneMesh : public Mesh, public CanQueryForHits {
public:
    PlaneMesh() = delete;

    PlaneMesh(Point center, Point normal) : center(center), normal(normal) {};

    AABB getSurroundingBox() const override {
        Point min{-consts::infinity,
                  -consts::infinity,
                  -consts::infinity};
        Point max{+consts::infinity,
                  +consts::infinity,
                  +consts::infinity};
        return AABB(min, max);
    }

    void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {
        float t = 0.0f;
        bool isFront = true;
        if(intersections::ray_plane(r, center, normal, t, isFront)){
            UV uvs = r.at(t) - center;
            n_intersections = 1;
            intersections[0] = {
                    r.at(t),
                    normal,
                    t,
                    uvs,
                    isFront
            };
        }
    }

    //Do not respect tmin with bboxes collision
    bool isHit(const Ray &r) const override {
        std::array<Intersection, 2> ins;
        int n_hits = 0;
        intersect(r, ins, n_hits);
        return n_hits > 0;
    }

public:

    Point center;
    Point normal;
};

