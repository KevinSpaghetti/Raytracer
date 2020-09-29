//
// Created by kevin on 15/08/20.
//
#pragma once

#include <memory>
#include <iostream>
#include <list>
#include "../Materials/Material.h"
#include "Transform.h"
#include "Hittable.h"
#include "../Geom/Intersections.h"
#include "../Mesh/Mesh.h"
#include "../BVH/Boxable.h"
#include "../BVH/AABB.h"

class Node : public Hittable, public Transform, public Boxable<AABB> {
public:
    Node() : mesh(std::make_shared<Mesh>()), material() {}

    Node(const std::shared_ptr<Mesh> mesh,
         const std::shared_ptr<Material> material) : mesh(mesh), material(material)
    {}

    std::vector<ObjectIntersection> hit(const Ray& r) override {
        std::vector<ObjectIntersection> intersections;

        //Apply the transform to the ray
        //The recursion applies the transforms without the need to explicitly multiply the matrices
        Ray t(pointToObjectSpace(r.getOrigin()),
              directionToObjectSpace(r.getDirection()));

        for(Intersection is : mesh->intersect(t)){
            //Push back the object with the coords in object space
            intersections.push_back({
                    is,
                    this
            });
        }

        for (auto child : children) {
            std::vector<ObjectIntersection> child_i = child->hit(t);
            intersections.insert(intersections.end(), child_i.begin(), child_i.end());
        }

        //Transform the intersection in world space coords
        for (ObjectIntersection& it : intersections) {
            it.pv = pointToWorldSpace(it.pv);
            it.pn = directionToWorldSpace(it.pn);
        }
        return intersections;
    }

    //Return the bounding box surrounding this node
    AABB getSurroundingBox() const override {
        return mesh->getSurroundingBox();
    }

    void add(const std::shared_ptr<Node> child){
        children.push_back(child);
    }
    void remove(const std::shared_ptr<Node> child){
        children.remove(child);
    }

    Mesh* getMesh(){
        return mesh.get();
    }
    Material* getMaterial(){
        return material.get();
    }
    std::list<std::shared_ptr<Node>> getChildren(){
        return children;
    }

    bool operator==(const Node rhs) const {
        return mesh == rhs.mesh
               && material == rhs.material
               && children == rhs.children;
    }

protected:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::list<std::shared_ptr<Node>> children;
};