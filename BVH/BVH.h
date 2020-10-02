//
// Created by kevin on 15/08/20.
//
#pragma once

#include <algorithm>
#include <iostream>
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//TODO: Build a better bvh by not using the ordering in the scene graph
//TODO: Fix rotations, it doesn't work with rotations in the scene because we cannot
//      rotate the AABB
//every bvh node is responsible for holding its bounding box and checking the intersection when necessary
//Not boxable since extern agents do not need to know that the class relies on bounding boxes
class BVH : public Hittable {
public:
    BVH(){};

    //Build the BVH from the scene graph
    BVH(Node root) : node(root){

        //Start with the mesh bounding box
        box = root.getSurroundingBox();
        for (const auto& child : root.getChildren()) {
            auto sub_box{std::make_shared<BVH>(*child)}; //Build the BVH subtree based on the child node n
            //Grow the node bounding box to contain all the children
            //bounding boxes
            box = AABB(box, sub_box->getSurroundingBox());
            children.push_back(sub_box); //Add the BVH subtree
        }
    }


    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        //Transform the ray from world space to object space
        Ray t(node.pointToObjectSpace(r.getOrigin()),
              node.directionToObjectSpace(r.getDirection()));

        //Check the ray hit against this node bounding box
        if(!box.isHit(t)) {
            //If the ray does not hit the hitbox it will not hit the object
            return ;
        }

        int tail = intersections.size();

        //If the ray hits the bounding box check
        //1. the object inside the bounding box
        std::vector<Intersection> mesh_intersections;
        node.getMesh()->intersect(t, mesh_intersections);
        for(Intersection is : mesh_intersections){
            intersections.push_back({
                    is,
                    &node
            });
        }

        //2. the children bounding boxes
        for (const auto& child : children){
            child->hit(t, intersections);
        }

        std::for_each(intersections.begin()+tail, intersections.end(), [this](auto it){
            it.pv = node.pointToWorldSpace(it.pv);
            it.pn = node.directionToWorldSpace(it.pn);
        });
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
