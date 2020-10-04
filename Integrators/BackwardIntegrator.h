//
// Created by Kevin on 18/09/2020.
//

#pragma once

#include <stack>
#include <utility>

#include "Integrator.h"
#include "../Geom/Hittable.h"
#include "../Utils/PDF.h"


class BackwardIntegrator : public Integrator {
public:

    struct SceneSamplerConfiguration{
        int max_ray_depth = 1;
        std::shared_ptr<Material> max_depth_material;
        std::shared_ptr<Material> no_hit_material;
    };

    BackwardIntegrator() = delete;
    BackwardIntegrator(Node* scene, SceneSamplerConfiguration  configuration) : scene(scene), configuration(std::move(configuration)) {


    }

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
        Ray ray{};
        Color incoming{0.0, 0.0, 0.0};

        if (material->scatter(intersection, r, ray)) {
            CosinePDF cs(intersection.pn);
            //ObjectPDF ob(nodelight, intersection.pv);
            MixPDF pdf(&cs, &cs);
            Normal dir = pdf.generate();
            float val = pdf.value(dir);
            incoming = trace(Ray(intersection.pv, dir), ++ray_depth);
            return material->color(intersection, r, incoming, val);
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

        intersection = *std::min_element(intersections.begin(), intersections.end(), [&r](auto i1, auto i2){
            return glm::length(r.getOrigin() - i1.pv) < glm::length(r.getOrigin() - i2.pv);
        });
        return true;
    }

    void getAllSceneIntersections(const Ray& r, std::vector<ObjectIntersection>& intersections) const {
        scene->hit(r, intersections);
    }

    Color maxRayDepthReached(const Ray& r) const {
        return configuration.max_depth_material->color({}, r, {0.0, 0.0, 0.0});
    }
    Color noIntersections(const Ray& r) const {
        return configuration.no_hit_material->color({}, r, {0.0, 0.0, 0.0});
    }

private:
    Node* scene;
    SceneSamplerConfiguration configuration;
};
