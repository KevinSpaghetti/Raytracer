//
// Created by kevin on 15/08/20.
//
#pragma once

#include <memory>
#include <iostream>
#include <list>
#include "Geometry.h"
#include "../Materials/Material.h"
#include "Transform.h"
#include "Hittable.h"
#include "../Geom/Intersections.h"
#include "../Geom/Sphere.h"
#include "Mesh.h"
#include "../BVH/Boxable.h"
#include "../BVH/AABB.h"

class Node : public Hittable, public Transform, public Boxable {
public:
    Node() : mesh(make_shared<Mesh>()), material() {}

    Node(const std::shared_ptr<Mesh> mesh,
         const std::shared_ptr<Material> material) : mesh(mesh), material(material)
    {}

    //TODO: Add a minimum distance to avoid self intersections
    std::list<ObjectIntersection> hit(const Ray& r) {
        std::list<ObjectIntersection> intersections;

        //Apply the transform to the ray
        Ray t(apply(glm::vec4(r.getOrigin(), 1.0)), r.getDirection());


        std::list<Intersection> i = mesh->intersect(t);
        for(Intersection is : i){
            ObjectIntersection ois{
                    is.pv,
                    is.pn,
                    is.uv,
                    material
            };
            intersections.push_back(ois);
        }

        std::list<ObjectIntersection> child_intersections;
        for (Node n : children) {
            n.mTransform = this->mTransform * n.mTransform;
            child_intersections = n.hit(r);
            intersections.splice(intersections.end(), child_intersections);
        }

        return intersections;
    }

    //Return the bounding box surrounding this node
    shared_ptr<BoundingBox> getSurroundingBox() override {
        return mesh->getSurroundingBox();
    }

    void add(const Node child){
        children.push_back(child);
    }
    void remove(const Node child){
        children.remove(child);
    }

    std::list<Node> getChildren(){
        return children;
    }
    std::shared_ptr<Material> getMaterial(){
        return material;
    }
    std::shared_ptr<Mesh> getMesh(){
        return mesh;
    }

    bool operator==(const Node rhs) const {
        return mesh == rhs.mesh
            && material == rhs.material
            && children == rhs.children;
    }

protected:
    std::shared_ptr<Material> material;
    std::list<Node> children;
    std::shared_ptr<Mesh> mesh;
};