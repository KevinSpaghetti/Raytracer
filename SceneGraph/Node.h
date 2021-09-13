//
// Created by kevin on 15/08/20.
//
#pragma once

#include <memory>
#include <iostream>
#include <list>
#include "../Materials/Material.h"
#include "../Geom/Transform.h"
#include "../Geom/Hittable.h"
#include "../Geom/Intersections.h"
#include "../Mesh/Mesh.h"
#include "../BVH/Boxable.h"
#include "../BVH/AABB.h"
#include "../Materials/DiffuseLight.h"
#include "../Utils/Random.h"


class Node : public Hittable {
public:
    enum class Type {
        Empty,
        Camera,
        Visual,
        Light
    };

    //Set global and local transforms to the identity transform
    Node() : local(LocalTransform()), global(LocalTransform()) {}

    virtual Type type() const = 0;

    //Since every node can have children the hit function
    //should at least check for the intersections with the children
    //every node needs to check the children because even if the ray does not hit
    //the empty node it can still contain children with meshes that can be hit
    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        for (const auto& child : children) {
            child->hit(r, intersections);
        }
    }

    void add(const std::shared_ptr<Node>& child){
        children.push_back(child);
    }
    void remove(const std::shared_ptr<Node>& child){
        children.remove(child);
    }

    void translate(const Direction& direction){
        local.translate(direction);
    };
    void rotate(const Normal& axis, const float radians){
        local.rotate(axis, radians);
    };
    void scale(const Dimension& dimensions){
        local.scale(dimensions);
    };

    LocalTransform transform_local() const {
        return local;
    }
    GlobalTransform transform_global() const {
        return global;
    }

    void set_transform_local(const LocalTransform& transform){
        local = transform;
    }
    void set_transform_global(const GlobalTransform& transform){
        global = transform;
    }

    std::list<std::shared_ptr<Node>> getChildren() const {
        return children;
    }

    //Compute all the node's global transforms
    static void computeGlobalTransforms(Node* root, const GlobalTransform& parent_global = GlobalTransform()){
        auto lcl = root->transform_local();

        root->set_transform_global(GlobalTransform::concat(parent_global, lcl));
        auto children = root->getChildren();
        for (const auto& child : children) {
            computeGlobalTransforms(child.get(), root->transform_global());
        }
    }

protected:
    //From object space to parent coordinate system
    //Must be set during scene creation
    LocalTransform local;
    //From object space to root coordinate system
    //Can be empty during scene creation, will be set from the
    //renderer
    GlobalTransform global;

    std::list<std::shared_ptr<Node>> children;
};
