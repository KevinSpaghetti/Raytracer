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
        //The bounding box along an axis is 0(near 0 after adding thickness) only if the normal
        //is parallel to the axis
        if(glm::dot(normal, consts::up) > 1.0f - 0.001){
            Point min{-consts::infinity,
                      0 - 0.01,
                      -consts::infinity};
            return AABB(min, -min);
        }
        if(glm::dot(normal, consts::left) > 1.0f - 0.001){
            Point min{0 - 0.01,
                      -consts::infinity,
                      -consts::infinity};
            return AABB(min, -min);
        }
        if(glm::dot(normal, consts::forward) > 1.0f - 0.001){
            Point min{-consts::infinity,
                      -consts::infinity,
                      0 - 0.01};
            return AABB(min, -min);
        }

        Point min{-consts::infinity,
                  -consts::infinity,
                  -consts::infinity};
        return AABB(min, -min);
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

