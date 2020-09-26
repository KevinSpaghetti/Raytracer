//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Mesh.h"

//TODO: refactor to make it work only for xy or xz aligned rects
class FlatRectMesh : public Mesh {
public:

    FlatRectMesh(const Point center, const Normal normal, const float width, const float height) :
            center(center),
            normal(normal),
            width(width),
            height(height) {}

    std::vector<Intersection> intersect(const Ray &r) const override {
        std::vector<Intersection> intersections;
        float t;
        bool isFront;
        if(intersections::ray_plane(r, center, normal, t, isFront)){
            intersections.push_back({
                    r.at(t),
                    normal,
                    {0, 0, 0}, //TODO: implement uvs
                    isFront
            });
            return intersections;
        }

        return intersections;
    }

    AABB getSurroundingBox() const override {
        const Point min = center - Point{width/2.0f, height/2.0f, 5*consts::epsilon};
        const Point max = center + Point{width/2.0f, height/2.0f, 5*consts::epsilon};
        return AABB(min, max);
    }

private:
    Point center;
    float width;
    float height;
    Normal normal;

};
