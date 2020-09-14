//
// Created by kevin on 27/08/20.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"
#include "../Geom/Intersection.h"
#include "../SceneGraph/IntersectTestable.h"

//Represents an empty mesh
//TODO: Meshes can have internal datastructures built
//     on construction to speed up the intersection tests
class Mesh : public IntersectTestable, public Boxable<AABB> {
public:
    Mesh() {}

    //To allow the empty mesh
    virtual std::list<Intersection> intersect(const Ray& r) const override {
        return std::list<Intersection>();
    }

    AABB getSurroundingBox() const override {
        return AABB();
    }

};

