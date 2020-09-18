//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include "Sampler.h"
#include "../SceneGraph/Hittable.h"

class SceneSampler : public Sampler {
public:

    struct SceneSamplerConfiguration{
        int max_ray_depth = 1;
        bool backface_culling = false;
    };

    SceneSampler() = delete;
    SceneSampler(Hittable* scene, const SceneSamplerConfiguration configuration) : scene(scene), configuration(configuration) {}

    Color sample(const Ray &r) const override {
        return trace(r, 0);
    }

private:
    //TODO: Add no hit materials and max depth reached materials
    //TODO: Remember the last BVH or Scene node hit and
    //      check that first
    Color trace(const Ray& r, int ray_depth) const {

        //If the ray has reached max depth then call the max depth shader
        if (ray_depth > configuration.max_ray_depth) {
            return maxRayDepthReached(r); //Return a background material color
        }

        //Check intersections with the scene
        std::list<ObjectIntersection> intersections;
        scene->hit(r, intersections);

        //If there are no intersections call the no hit shader
        if (intersections.empty()) {
            return noIntersections(r);
        }

        //If backface culling is activated pick the intersection
        //with a face that is front
        ObjectIntersection intersection;
        if (configuration.backface_culling) {
            intersection = intersections.front();
            while (intersections.empty() || intersection.isFront) {
                intersections.pop_front();
            }
            if (intersections.empty()) {
                noIntersections(r);
            }
        } else {
            auto comparator = [&r](ObjectIntersection i1, ObjectIntersection i2){
                return glm::length(r.getOrigin() - i1.pv) < glm::length(r.getOrigin() - i2.pv);
            };
            intersection = *std::min_element(intersections.begin(), intersections.end(), comparator);
        }

        //TODO: Pass a sampler to the material with the ability to trace rays
        std::shared_ptr<Material> material = intersection.node->getMaterial();
        Ray ray({0, 0, 0}, {0, 0, 0});
        Color incoming{0, 0, 0};
        if (material->scatter(intersection, r, ray)) {
            incoming = trace(ray, ray_depth + 1);
        }

        return material->color(intersection, r, incoming);
    }

    Color maxRayDepthReached(const Ray& r) const {
        return Color{1.0, 0, 0};
    }

    Color noIntersections(const Ray& r) const {
        Point unit_direction = r.getDirection();
        float t = 0.5f * (unit_direction.y + 1.0);
        return (1.0f - t) * Color(1.0) + t * Color{0.5, 0.7, 1.0};
    }

private:
    Hittable* scene;
    SceneSamplerConfiguration configuration;
};
