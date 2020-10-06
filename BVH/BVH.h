//
// Created by kevin on 15/08/20.
//
#pragma once

#include <algorithm>
#include <iostream>
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"

//TODO: move
AABB applyTransform(AABB box, GlobalTransform t){
    Dimension extent = box.getMax() - box.getMin();
    Point min = box.getMin();
    Point max = box.getMax();
    std::vector<Point> points({
              t.pointToWorldSpace(min),
              t.pointToWorldSpace(min + Point{extent.x, 0.0f, 0.0f}),
              t.pointToWorldSpace(min + Point{0.0f, extent.y, 0.0f}),
              t.pointToWorldSpace(min + Point{0.0f, 0.0f, extent.z}),
              t.pointToWorldSpace(max - Point{extent.x, 0.0f, 0.0f}),
              t.pointToWorldSpace(max - Point{0.0f, extent.y, 0.0f}),
              t.pointToWorldSpace(max - Point{0.0f, 0.0f, extent.z}),
              t.pointToWorldSpace(max)
    });
    return AABB(points);
}

//TODO: Build a better bvh by not using the ordering in the scene graph
//TODO: Fix rotations, it doesn't work with rotations in the scene because we cannot
//      rotate the AABB
//every bvh node is responsible for holding its bounding box and checking the intersection when necessary
//Not boxable since extern agents do not need to know that the class relies on bounding boxes
class BVH : public Hittable {
public:

    BVH(std::vector<VisualNode*> nodes){

        if(nodes.size() == 1){
            box = applyTransform(nodes[0]->getSurroundingBox(), nodes[0]->transform_global());
            node = nodes[0];
            left = nullptr;
            right = nullptr;
            return ;
        }
        if(nodes.size() == 2){
            AABB b1 = applyTransform(nodes[0]->getSurroundingBox(), nodes[0]->transform_global());
            AABB b2 = applyTransform(nodes[1]->getSurroundingBox(), nodes[1]->transform_global());
            box = b1;
            box.grow(b2);
            node = nullptr;
            left = std::make_unique<BVH>(std::vector<VisualNode*>({nodes[0]}));
            right = std::make_unique<BVH>(std::vector<VisualNode*>({nodes[1]}));
            return ;
        }

        node = nullptr;
        //Build this node bounding box
        box = AABB();
        for (const auto& n : nodes) {
            //Transform the bounding box in world space
            //(Need to reconstruct if its an AABB for rotations)
            AABB child_box = applyTransform(n->getSurroundingBox(), n->transform_global());
            box = AABB(box, child_box);
        }
        //Decide which nodes need to go in the left and right bins
        int axis = static_cast<int>(randomized::scalar::random(0, 2)); //Split along a random axis
        std::sort(nodes.begin(), nodes.end(), [axis](const VisualNode* n1, const VisualNode* n2) -> bool {
            AABB node_box1 = applyTransform(n1->getSurroundingBox(), n1->transform_global());
            AABB node_box2 = applyTransform(n2->getSurroundingBox(), n2->transform_global());
            return node_box1.getCenter()[axis] < node_box2.getCenter()[axis];
        });

        const int half = nodes.size() / 2;
        std::vector<VisualNode*> nodes_left = std::vector<VisualNode*>(nodes.begin(), nodes.begin() + half);
        std::vector<VisualNode*> nodes_right = std::vector<VisualNode*>(nodes.begin() + half, nodes.end());

        if(nodes_left.empty()){
            left = nullptr;
        }else{
            left = std::make_unique<BVH>(nodes_left);
        }
        if(nodes_left.empty()){
            right = nullptr;
        }else{
            right = std::make_unique<BVH>(nodes_right);
        }
    };

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {


        if(node != nullptr){
            //Apply the transform to the ray
            Ray t(node->transform_global().pointToObjectSpace(r.getOrigin()),
                  node->transform_global().directionToObjectSpace(r.getDirection()),
                  r.getType());

            std::vector<Intersection> mesh_intersections;
            node->getMesh()->intersect(t, mesh_intersections);
            for (Intersection &is : mesh_intersections) {
                //Push back the object with the coords in object space
                ObjectIntersection o{};
                o.point = is.point;
                o.normal = is.normal;
                o.ws_point = node->transform_global().pointToWorldSpace(is.point);
                o.ws_normal = node->transform_global().directionToWorldSpace(is.normal);
                o.uv = is.uv;
                o.node = node;
                intersections.push_back(o);
            }
        }

        //Check the children nodes in the parent
        //to allow SIMD operations
        if(left != nullptr){
            left->hit(r, intersections);
        }
        if(right != nullptr){
            right->hit(r, intersections);
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
    AABB box = AABB();
    VisualNode* node = nullptr;

    std::unique_ptr<BVH> left;
    std::unique_ptr<BVH> right;
};
