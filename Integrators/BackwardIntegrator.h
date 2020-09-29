//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include <stack>

#include "Integrator.h"
#include "../SceneGraph/Hittable.h"

class BackwardIntegrator : public Integrator {
public:

    struct SceneSamplerConfiguration{
        int max_ray_depth = 1;
        bool backface_culling = false;
    };

    BackwardIntegrator() = delete;
    BackwardIntegrator(Hittable* scene, const SceneSamplerConfiguration configuration) : scene(scene), configuration(configuration) {}

    Color sample(const Ray &r) const override {
        return trace(r, 0);
    }

private:
    //TODO: Add no hit materials and max depth reached materials
    //TODO: Remember the last BVH or Scene node hit and
    //      check that first
    //Iterative version is more extensible
    Color trace(const Ray& r, int ray_depth) const {

        if(ray_depth > configuration.max_ray_depth){
            return maxRayDepthReached(r);
        }

        //If there are no intersections call the no hit shader
        ObjectIntersection intersection{};
        if (!getClosestSceneIntersection(r, intersection)) {
            return noIntersections(r);
        }

        //TODO: Pass a sampler to the material with the ability to trace rays
        auto material = intersection.node->getMaterial();
        Ray ray{};
        Color incoming{0.0, 0.0, 0.0};
        if (material->scatter(intersection, r, ray)) {
            incoming = trace(ray, ++ray_depth);
        }
        return material->color(intersection, r, incoming);
    }

protected:

    //We use vector and not lists because:
    //  - Less cache misses
    //  - Memory can be allocated in bigger chunks
    //  - Memory can be freed all at once for a vector, the list deletes every node
    //  - (Huge performance bottleneck for lots of sparse allocs and deallocs, from 24s to 2s)
    //We pass the vector of intersections and not return it so that it leads to more cache
    //coherence because it writes always in the same memory, and doesn't need to copy or move elements on return

    bool getClosestSceneIntersection(const Ray& r, ObjectIntersection& intersection) const {

        //Check intersections with the scene
        std::vector<ObjectIntersection> intersections;
        getAllSceneIntersections(r, intersections);

        if(intersections.empty()){
            return false;
        }

        //If backface culling is activated pick the intersection
        //with a face that is front
        if (configuration.backface_culling) {
            //TODO: implement ;
        } else {
            intersection = *std::min_element(intersections.begin(), intersections.end(), [&r](auto i1, auto i2){
                return glm::length(r.getOrigin() - i1.pv) < glm::length(r.getOrigin() - i2.pv);
            });
        }
        return true;
    }

    void getAllSceneIntersections(const Ray& r, std::vector<ObjectIntersection>& intersections) const {
        intersections = scene->hit(r);
    }

    Color maxRayDepthReached(const Ray& r) const {
        return Color{1.0, 0.0, 0.0};
    }

    Color noIntersections(const Ray& r) const {
        //return Color{0.0, 0.0, 0.0};
        Point unit_direction = r.getDirection();
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * Color(1.0) + t * Color{0.5, 0.7, 1.0};
    }

private:
    Hittable* scene;
    SceneSamplerConfiguration configuration;
};
