//
// Created by kevin on 15/08/20.
//
#pragma once

#include <algorithm>
#include <iostream>
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//TODO: Build a better bvh by not using the ordering in the scene graph
//TODO: Apply transforms to the objects instead of multiplying matrices every ray
class BVH : public Node {
public:
    BVH(){};

    //Build the BVH from the scene graph
    BVH(Node root){
        mesh = root.getMesh();
        shader = root.getShader();
        mTransform = root.getTransform();

        //Start with the mesh bounding box
        box = root.getSurroundingBox();
        for (Node n : root.getChildren()) {
            //Build the BVH subtree based on the child node n
            BVH sub_box(n);
            //Get the BVH subtree bounding box
            std::shared_ptr<BoundingBox> bx = sub_box.getSurroundingBox();
            //Grow the node bounding box to contain all the children
            //bounding boxes
            box = std::make_shared<AABB>(box, bx);
            children.push_back(make_shared<BVH>(sub_box));
        }
    }


    std::list<ObjectIntersection> hit(const Ray& r) {
        std::list<ObjectIntersection> intersections;

        Ray t(apply(glm::vec4(r.getOrigin(), 1.0)), r.getDirection());

        //Check the ray hit against this node bounding box
        if(!box->isHit(t)) {
            //If the ray does not hit the hitbox it will not hit the object
            return intersections;
        }

        //If the ray hits the bounding box check
        //1. the object inside the bounding box
        //We cannot use the hit function from the superclass node
        //since it checks the subnodes which we do not want
        std::list<Intersection> i = mesh->intersect(t);
        for(Intersection is : i){
            intersections.push_back({
                    is.pv,
                    is.pn,
                    is.uv,
                    shader
            });
        }

        //2. the children bounding boxes
        for (const std::shared_ptr<BVH>& n : children){
            n->mTransform = this->mTransform * n->mTransform;
            std::list<ObjectIntersection> is = n->hit(t);
            intersections.splice(intersections.end(), is);
        }

        ObjectIntersection closest = intersections.front();
        for(ObjectIntersection i : intersections){
            if(glm::length(r.getOrigin() - i.pv) < glm::length(r.getOrigin() - closest.pv)){
                closest = i;
            }
        }
        //Return only the closest intersection to the camera origin

        return std::list<ObjectIntersection>({closest});
    }

    shared_ptr<BoundingBox> getSurroundingBox() override {
        return box;
    }

protected:
    shared_ptr<BoundingBox> box;
    std::list<shared_ptr<BVH>> children;
};
