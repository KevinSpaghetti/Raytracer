//
// Created by kevin on 15/10/20.
//

#pragma once

#include "Node.h"
#include "../BVH/AABB.h"
#include "../Mesh/Mesh.h"
#include "../Materials/Material.h"

class VisualNode : public Node, public Boxable<AABB> {
public:
    VisualNode(const std::shared_ptr<Mesh> mesh, const std::shared_ptr<Material> material)
            : mesh(mesh), material(material) {}

    Type type() const override {
        return Type::Visual;
    }

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        //Apply the transform to the ray
        //The recursion applies the transforms without the need to explicitly multiply the matrices
        Ray t(global.pointToObjectSpace(r.getOrigin()),
              global.directionToObjectSpace(r.getDirection()), r.getType());

        std::array<Intersection, 2> mesh_intersections;
        int n_ins;
        mesh->intersect(t, mesh_intersections, n_ins);
        for(auto is = mesh_intersections.begin(); is != mesh_intersections.begin() + n_ins; ++is){
            //Push back the object with the coords in object space
            ObjectIntersection o{};
            o.point = is->point;
            o.normal = is->normal;
            o.ws_point = global.pointToWorldSpace(is->point);
            o.ws_normal = global.directionToWorldSpace(is->normal);
            o.uv = is->uv;
            o.t = is->t;
            o.isFront = is->isFront;
            o.node = this;
            intersections.push_back(o);
        }

        for (auto child : children) {
            child->hit(r, intersections);
        }

    }

    //Return the bounding box surrounding this node
    //with respect to object space
    AABB getSurroundingBox() const override {
        return mesh->getSurroundingBox();
    }


    Mesh* getMesh() const{
        return mesh.get();
    }
    Material* getMaterial() const{
        return material.get();
    }

protected:
    VisualNode() {}

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};
