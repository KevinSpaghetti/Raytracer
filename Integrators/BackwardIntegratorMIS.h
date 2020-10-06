//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include <stack>
#include <utility>

#include "Integrator.h"
#include "../Geom/Hittable.h"
#include "../Utils/PDF.h"
#include "../SceneGraph/SceneList.h"
#include "BackwardIntegrator.h"

//Backward integrator with multiple importance sampling capabilities
class BackwardIntegratorMIS : public BackwardIntegrator {
public:

    BackwardIntegratorMIS() = delete;
    BackwardIntegratorMIS(SceneList* scene, SceneSamplerConfiguration  configuration) : BackwardIntegrator(scene, configuration), scene(scene), configuration(std::move(configuration)) {}

    Color sample(const Ray &r) const override {
        return trace(r, 0);
    }

private:
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

        //An intersection can occur only on a VisualNode node type or its
        //subclasses
        const auto* node = dynamic_cast<const VisualNode*>(intersection.node);
        auto material = node->getMaterial();
        Color emitted{0.0, 0.0, 0.0};
        Color incoming{0.0, 0.0, 0.0};
        if (material->emits(intersection, r)){
            emitted = material->emit(intersection, r);
        }
        //If the material does not scatter light than the rendering equation 2nd term is always 0
        if (!material->scatters(intersection, r)) {
            return emitted;
        }

        Ray scattered = material->scatter(intersection, r);
        incoming = trace(scattered, ++ray_depth);

        Color brdf = material->f(intersection, r, scattered);

        //Compute the rendering equation with the material parameters
        return emitted + (brdf * incoming * glm::dot( -r.getDirection(), scattered.getDirection()));
    }


private:
    SceneList* scene;
    SceneSamplerConfiguration configuration;
};
