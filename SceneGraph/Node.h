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

class Node : public Hittable, public Transform {
public:
    Node() = default;

    Node(const std::shared_ptr<Sphere> sphere,
         const std::shared_ptr<Material> material) : sphere(sphere), material(material)
    {}

    std::list<Intersection> hit(const Ray& r, double t_min, double t_max) {
        std::list<Intersection> intersections;

        hit_record rec{};
        if(sphere->hit(r, 0.001, infinity, rec)){
            Intersection i{
                Vertex{rec.p.x, rec.p.y, rec.p.z},
                Normal{rec.normal.x, rec.normal.y, rec.normal.z},
                material
            };

            intersections.push_back(i);
        }

        std::list<Intersection> child_intersections;
        for (Node n : children) {
            child_intersections = n.hit(r, t_min, t_max);
            intersections.splice(intersections.end(), child_intersections);
        }

        return intersections;
    }

    void add(const Node child){
        children.push_back(child);
    }
    void remove(const Node child){
        children.remove(child);
    }

    bool operator==(const Node rhs) const {
        return sphere == rhs.sphere
            && material == rhs.material
            && children == rhs.children;
    }

    //Return a copy of the node with the transform applied
    Node transform(){

        std::shared_ptr<Sphere> sp = std::make_shared<Sphere>(apply(glm::vec4(sphere->center, 1.0f)), sphere->radius, sphere->mat_ptr);
        Node parent(sp, material);
        for (Node child : children) {
            child.mTransform = child.mTransform * this->mTransform;
            parent.add(child.transform());
        }
        return parent;
    }

    std::shared_ptr<Material> getMaterial() const {
        return material;
    }

protected:
    std::shared_ptr<Sphere> sphere;
    std::shared_ptr<Material> material;
    std::list<Node> children;
};