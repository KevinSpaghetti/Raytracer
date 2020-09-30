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

};

