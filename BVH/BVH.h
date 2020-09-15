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
class BVH : public Hittable{
public:
    BVH(){};

    //Build the BVH from the scene graph
    BVH(Node root) : node(root){

        //Start with the mesh bounding box
        box = root.getSurroundingBox();
        for (shared_ptr<Node> child : root.getChildren()) {
            BVH sub_box(*child); //Build the BVH subtree based on the child node n
            children.push_back(make_shared<BVH>(sub_box)); //Add the BVH subtree
            //Grow the node bounding box to contain all the children
            //bounding boxes
            box = AABB(box, sub_box.getSurroundingBox());
        }
    }


    std::list<ObjectIntersection> hit(const Ray& r) override {
        std::list<ObjectIntersection> intersections;

        //Transform the ray from world space to object space
        Ray t(node.pointToObjectSpace(r.getOrigin()),
              node.directionToObjectSpace(r.getDirection()));

        //Check the ray hit against this node bounding box
        if(!box.isHit(t)) {
            //If the ray does not hit the hitbox it will not hit the object
            return intersections;
        }

        //If the ray hits the bounding box check
        //1. the object inside the bounding box
        std::list<Intersection> i = node.getMesh()->intersect(t);
        for(Intersection is : i){
            intersections.push_back({
                    is,
                    &node
            });
        }

        //2. the children bounding boxes
        for (const std::shared_ptr<BVH>& child : children){
            std::list<ObjectIntersection> is = child->hit(t);
            intersections.splice(intersections.end(), is);
        }

        //Transform the intersections from object space back to world space
        for (ObjectIntersection& it : intersections) {
            it.pv = node.pointToWorldSpace(it.pv);
            it.pn = node.directionToWorldSpace(it.pn);
        }

        return intersections;
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
    std::list<shared_ptr<BVH>> children;
};
