//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include <stack>
#include <utility>

#include "Integrator.h"
#include "../Geom/Hittable.h"
#include "../SceneGraph/SceneList.h"


class BackwardIntegrator : public Integrator {
public:

    struct SceneSamplerConfiguration{
        int max_ray_depth = 1;
        std::shared_ptr<Material> max_depth_material;
        std::shared_ptr<Material> no_hit_material;
    };

    BackwardIntegrator(Hittable* scene, SceneSamplerConfiguration  configuration) : scene(scene), configuration(std::move(configuration)) {}

    Color sample(const Ray &r) const override {
        return trace(r, 0);
    }

private:
    virtual Color trace(const Ray& r, int ray_depth) const {

        if(ray_depth > configuration.max_ray_depth){
            return maxRayDepthReached(r);
        }

        //If there are no intersections call the no hit shader
        ObjectIntersection intersection{};
        if (!getClosestSceneIntersection(r, intersection)) {
            return noIntersections(r);
        }

        //An intersection can occur only on a VisualNode node type or its
        //subclasses
        const auto* node = dynamic_cast<const VisualNode*>(intersection.node);
        auto material = node->getMaterial();

        //Direction where the light is going (to the eye)
        const Ray wo = Ray(intersection.ws_point, -r.getDirection());

        Color emitted{0.0, 0.0, 0.0};
        if (material->emits(intersection, wo)){
            emitted = material->emit(intersection, wo);
        }
        //If the material does not scatter light then the rendering equation 2nd term is always 0
        Ray scattered{};
        Color incoming{0.0, 0.0, 0.0};
        if (material->scatters(intersection, wo)) {
            scattered = material->scatter(intersection, wo);
            incoming = trace(scattered, ++ray_depth);
        }

        const float p = 1.0f;

        //Direction where the light is coming (to the light)
        const Ray wi = scattered;
        Color brdf = material->f(intersection, wi, wo);
        //Compute the rendering equation with the material parameters
        return emitted + (brdf * incoming);
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
        if(intersections.size() == 1){
            intersection = intersections[0];
            return true;
        }
        intersection = *std::min_element(intersections.begin(), intersections.end(), [](auto i1, auto i2){
            return i1.t < i2.t;
        });
        return true;
    }

    void getAllSceneIntersections(const Ray& r, std::vector<ObjectIntersection>& intersections) const {
        scene->hit(r, intersections);
    }

    Color maxRayDepthReached(const Ray& r) const {
        return configuration.max_depth_material->emit(Intersection{}, r);
    }
    Color noIntersections(const Ray& r) const {
        return configuration.no_hit_material->emit(Intersection{}, r);
    }

protected:
    Hittable* scene;
    SceneSamplerConfiguration configuration;
};
