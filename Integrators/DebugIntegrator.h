//
// Created by kevin on 13/10/20.
//

#pragma once

#include "BackwardIntegrator.h"

class DebugIntegrator : public BackwardIntegrator {
public:
    DebugIntegrator() = delete;
    using BackwardIntegrator::BackwardIntegrator; //Inherit the base class constructor

    //Tecnique derived from https://graphics.pixar.com/library/RendermanTog2018/paper.pdf (Integrators for simple rendering)
    //We just need to sample the scene normals
    Color sample(const Ray &r) const override {

        //If there are no intersections call the no hit shader
        ObjectIntersection intersection;
        if (!getClosestSceneIntersection(r, intersection)) {
            return noIntersections(r);
        }

        return intersection.ws_point;
    }

};