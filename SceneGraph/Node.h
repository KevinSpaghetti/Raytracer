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

class Node : public Hittable {
public:
    Node() = default;

    Node(const std::shared_ptr<Geometry> geometry,
         const std::shared_ptr<Material> material,
         const std::shared_ptr<Transform> transform = std::make_shared<Transform>())
         : geometry(geometry), material(material)
    {}

    std::list<Intersection> hit(const Ray& r, double t_min, double t_max) {
        std::list<Intersection> intersections;

        //Intersection tests for the triangles in the geometry
        //If there is no geometry then the vector will be empty
        for(Triangle tri : geometry->triangles){
            Vertex v1 = geometry->vertices[tri.a];
            Vertex v2 = geometry->vertices[tri.b];
            Vertex v3 = geometry->vertices[tri.c];
            float u, v, t;
            if(intersections::intersect(r, v1.vec(), v2.vec(), v3.vec(), u, v, t)){
                Normal in{};
                if (geometry->normals.size() > 0){
                    Normal n1 = geometry->normals[tri.a];
                    Normal n2 = geometry->normals[tri.b];
                    Normal n3 = geometry->normals[tri.c];
                    glm::vec3 vin = u * n1.vec() + v * n2.vec() + (1 - u - v)*n3.vec();
                    in = {vin.x, vin.y, vin.z};
                }else{
                    in = {0, 0, 0};
                }
                glm::vec3 iv = u * v1.vec() + v * v2.vec() + (1 - u - v)*v3.vec();
                Intersection i{
                    Vertex{iv.x, iv.y, iv.z},
                    in,
                };
                intersections.push_back(i);
            }
        }

        return intersections;
    }

    void add(const Node child){
        children->push_back(child);
    }
    void remove(const Node child){
        children->remove(child);
    }

    bool operator==(Node rhs){
        return geometry == rhs.geometry
            && transform == rhs.transform
            && material == rhs.material
            && children == rhs.children;
    }

protected:
    std::shared_ptr<Geometry> geometry;
    std::shared_ptr<Transform> transform;
    std::shared_ptr<Material> material;
    std::shared_ptr<std::list<Node>> children;
};