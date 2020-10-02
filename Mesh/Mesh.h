//
// Created by kevin on 27/08/20.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"
#include "../Geom/Intersection.h"
#include "../SceneGraph/IntersectTestable.h"
#include "../BVH/AABB.h"

//Represents an empty mesh
// Meshes can have internal datastructures built
//     on construction to speed up the intersection tests
class Mesh : public IntersectTestable, public Boxable<AABB> {
public:
    Mesh() {}

    //To allow the empty mesh
    virtual void intersect(const Ray& r, std::vector<Intersection>& intersections) const override {
        return ;
    }

    AABB getSurroundingBox() const override {
        return AABB();
    }

    virtual double pdf(const Point& origin, const Normal& direction) const {
        return 0.0f; //The ray will never get scattered in any direction
    }
    virtual Point random(const Point& o) const {
        return Point{1, 0, 0};
    }

};

