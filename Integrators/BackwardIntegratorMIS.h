//
// Created by kevin on 13/10/20.
//

#pragma once

#include "BackwardIntegrator.h"

class BackwardIntegratorMIS : public BackwardIntegrator {


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

        //Direction where the light is going (to the eye)
        const Ray wo = Ray(intersection.ws_point, -r.getDirection());

        Color emitted{0.0, 0.0, 0.0};
        if (material->emits(intersection, wo)){
            emitted = material->emit(intersection, wo);
        }
        //If the material does not scatter light than the rendering equation 2nd term is always 0
        Ray scattered{};
        Color incoming{0.0, 0.0, 0.0};
        if (material->scatters(intersection, wo)) {
            scattered = material->scatter(intersection, wo);
            incoming = trace(scattered, ++ray_depth);
        }

        //Direction where the light is coming (to the light)
        const Ray wi = scattered;
        Color brdf = material->f(intersection, wi, wo);
        //Compute the rendering equation with the material parameters
        float lambert_law = abs(glm::dot(wi.getDirection(), glm::normalize(intersection.ws_normal)));
        return emitted + (brdf * incoming * lambert_law);
    }
};

