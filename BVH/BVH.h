//
// Created by kevin on 15/08/20.
//
#pragma once

#include <algorithm>
#include <iostream>
#include "../Geom/Intersection.h"
#include "../Geom/ObjectIntersection.h"
#include "../SceneGraph/VisualNode.h"

class BVH : public Hittable {
public:

    BVH(std::vector<VisualNode*> nodes){
        //Build this node bounding box
        box = AABB();
        for (const auto& n : nodes) {
            //Transform the bounding box in world space
            //(Need to reconstruct if its an AABB for rotations)
            AABB object_space_box = n->getSurroundingBox();
            GlobalTransform node_transform = n->transform_global();
            AABB child_box = applyTransform(object_space_box, node_transform);
            box.grow(child_box);
        }

        if(nodes.size() == 1){
            node = nodes[0];
            left = nullptr;
            right = nullptr;
            return ;
        }
        if(nodes.size() == 2){
            node = nullptr;
            left = std::make_unique<BVH>(std::vector<VisualNode*>({nodes[0]}));
            right = std::make_unique<BVH>(std::vector<VisualNode*>({nodes[1]}));
            return ;
        }

        node = nullptr;
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

        if(!box.isHit(r)){
            return ;
        }

        if(node != nullptr){
            //Apply the transform to the ray
            GlobalTransform gl = node->transform_global();
            Ray t(gl.pointToObjectSpace(r.getOrigin()),
                  gl.directionToObjectSpace(r.getDirection()),
                  r.getType());

            std::array<Intersection, 2> mesh_intersections;
            int n_ins = 0;
            node->getMesh()->intersect(t, mesh_intersections, n_ins);
            if(n_ins > 0) {
                for (int i = 0; i < n_ins; ++i) {
                    //Push back the object with the coords in object space
                    Intersection is = mesh_intersections[i];
                    ObjectIntersection o{};
                    o.point = is.point;
                    o.normal = is.normal;
                    o.ws_point = node->transform_global().pointToWorldSpace(is.point);
                    o.ws_normal = glm::normalize(node->transform_global().directionToWorldSpace(is.normal));
                    o.uv = is.uv;
                    o.isFront = is.isFront;
                    o.t = is.t;
                    o.node = node;
                    intersections.push_back(o);
                }
            }

        }

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
