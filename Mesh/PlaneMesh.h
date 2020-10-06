//
// Created by kevin on 15/08/20.
//

#pragma once

#include "../Mesh/Mesh.h"
#include "../BVH/Boxable.h"

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

    void intersect(const Ray& r, std::vector<Intersection>& intersections) const override {
        float t = 0.0f;
        bool isFront = true;
        if(intersections::ray_plane(r, center, normal, t, isFront)){
            intersections.push_back({
                    r.at(t),
                    normal,
                    {0, 0, 0}, //TODO: implement uvs
                    isFront
            });
        }
    }

    //Do not respect tmin with bboxes collision
    bool isHit(const Ray &r) const override {
        std::vector<Intersection> ins;
        intersect(r, ins);
        return ins.size() > 0;
    }

public:

    Point center;
    Point normal;
};

