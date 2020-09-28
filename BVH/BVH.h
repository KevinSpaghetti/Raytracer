//
// Created by kevin on 15/08/20.
//
#pragma once

#include <algorithm>
#include <iostream>
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//TODO: Build a better bvh by not using the ordering in the scene graph
//TODO: every bvh node is responsible for holding its bounding box and checking the intersection when necessary
//Not boxable since extern agents do not need to know that the class relies on bounding boxes
class BVH : public Hittable {
public:
    BVH(){};

    //Build the BVH from the scene graph
    BVH(Node root) : node(root){

        //Start with the mesh bounding box
        box = root.getSurroundingBox();
        for (const auto& child : root.getChildren()) {
            BVH sub_box(*child); //Build the BVH subtree based on the child node n
            children.push_back(std::make_shared<BVH>(sub_box)); //Add the BVH subtree
            //Grow the node bounding box to contain all the children
            //bounding boxes
            box = AABB(box, sub_box.getSurroundingBox());
        }

    }


    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) override {
        //Transform the ray from world space to object space
        Ray t(node.pointToObjectSpace(r.getOrigin()),
              node.directionToObjectSpace(r.getDirection()));

        //Check the ray hit against this node bounding box
        if(!box.isHit(t)) {
            //If the ray does not hit the hitbox it will not hit the object
            return;
        }

        //If the ray hits the bounding box check
        //1. the object inside the bounding box
        for(Intersection is : node.getMesh()->intersect(t)){
            intersections.push_back({
                    is,
                    &node
            });
        }

        //2. the children bounding boxes
        for (const auto& child : children){
            child->hit(t, intersections);
        }

        //Transform the intersections from object space back to world space
        for (ObjectIntersection& it : intersections) {
            it.pv = node.pointToWorldSpace(it.pv);
            it.pn = node.directionToWorldSpace(it.pn);
        }
    }

private:
    //Since now bounding box is private we can use our
    //concrete implementation for the bbox returned
    //Return by copy since AABB is inexpensive
    AABB getSurroundingBox() {
        return box;
    }

protected:
    AABB box;
    Node node;
    std::vector<std::shared_ptr<BVH>> children;
};
