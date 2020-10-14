//
// Created by kevin on 05/10/20.
//

#pragma once

#include "../Geom/Hittable.h"
#include "../Utils/PDF.h"

class SceneList : public Hittable, public ImportanceSamplingEnabled {
public:
    SceneList(const std::vector<VisualNode*> objects, const std::vector<LightNode*> lights)
        : visuals(objects),
          lights(lights) {}

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {
        for (const auto object : visuals) {

            Ray t(object->transform_global().pointToObjectSpace(r.getOrigin()),
                  object->transform_global().directionToObjectSpace(r.getDirection()), r.getType());

            std::array<Intersection, 2> mesh_intersections;
            int n_ins = 0;
            object->getMesh()->intersect(t, mesh_intersections, n_ins);
            for(auto is = mesh_intersections.begin(); is != mesh_intersections.begin() + n_ins; ++is){
                //Push back the object with the coords in object space
                ObjectIntersection o{};
                o.point = is->point;
                o.normal = is->normal;
                o.ws_point = object->transform_global().pointToWorldSpace(is->point);
                o.ws_normal = object->transform_global().directionToWorldSpace(is->normal);
                o.uv = is->uv;
                o.t = is->t;
                o.isFront = is->isFront;
                o.node = object;
                intersections.push_back(o);
            }
        }
    }

    //For now when doing importance sampling pick a random light
    //Return a random ray towards a random light
    Color random(const Point &origin) const override {
        int random_index = static_cast<int>(randomized::scalar::random(0, lights.size()));
        LightNode *light = lights[random_index];
        Ray r = light->randomPoint(origin);

        ObjectIntersection intersection;
        if(getClosestSceneIntersection(r, intersection)){
            if(intersection.node == light){
                //Calculate the light contribution
                return Color{1.0, 1.0, 1.0};
            }else{
                return Color{0, 0.0, 0};
            }
        }
        return Color{0.0, 0, 0};
    }

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
