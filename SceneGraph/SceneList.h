//
// Created by kevin on 05/10/20.
//

#pragma once

#include "../Geom/Hittable.h"
#include "LightNode.h"

//Primitive implementation of a scene graph where all the objects are store flatly in a list

class SceneList : public Hittable {
public:
    SceneList(std::vector<VisualNode*>&& objects, std::vector<LightNode*>&& lights)
        : visuals(std::forward<std::vector<VisualNode*>>(objects)),
          lights(std::forward<std::vector<LightNode*>>(lights)) {}

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {
        for (const auto object : visuals) {

            Ray t(object->transform_global().pointToObjectSpace(r.getOrigin()),
                  object->transform_global().directionToObjectSpace(r.getDirection()), r.getType());

            std::array<Intersection, 2> mesh_intersections;
            int n_ins = 0;
            object->getMesh()->intersect(t, mesh_intersections, n_ins);
            if(n_ins > 0) {
                for (int i = 0; i < n_ins; ++i) {
                    //Push back the object with the coords in object space
                    Intersection is = mesh_intersections[i];
                    ObjectIntersection o{};
                    o.point = is.point;
                    o.normal = is.normal;
                    o.ws_point = object->transform_global().pointToWorldSpace(is.point);
                    o.ws_normal = glm::normalize(object->transform_global().directionToWorldSpace(is.normal));
                    o.uv = is.uv;
                    o.isFront = is.isFront;
                    o.t = is.t;
                    o.node = object;
                    intersections.push_back(o);
                }
            }
        }
    }

    LightNode* getLight(const ObjectIntersection& p) const {
        int random_index = static_cast<int>(randomized::scalar::random(0, lights.size()));
        LightNode *light = lights[random_index];
        return light;
    };

private:
    bool getClosestSceneIntersection(const Ray& r, ObjectIntersection& intersection) const {

        //Check intersections with the scene
        std::vector<ObjectIntersection> intersections;
        hit(r, intersections);

        if(intersections.empty()){
            return false;
        }

        intersection = *std::min_element(intersections.begin(), intersections.end(), [&r](auto i1, auto i2){
            return glm::length(r.getOrigin() - i1.point) < glm::length(r.getOrigin() - i2.point);
        });
        return true;
    }

private:
    std::vector<VisualNode*> visuals;
    std::vector<LightNode*> lights;
};
