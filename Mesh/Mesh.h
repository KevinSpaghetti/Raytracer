//
// Created by kevin on 27/08/20.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"
#include "../Geom/Intersection.h"
#include "../Geom/IntersectTestable.h"
#include "../BVH/AABB.h"

//Represents an empty mesh
// Meshes can have internal datastructures built
//     on construction to speed up the intersection tests
class Mesh : public IntersectTestable, public Boxable<AABB> {
public:
    Mesh() {}

    //To allow the empty mesh
    virtual void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {
        n_intersections = 0;
        return ;
    }

    AABB getSurroundingBox() const override {
        return AABB();
    }


};

