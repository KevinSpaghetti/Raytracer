//
// Created by Kevin on 19/09/2020.
//

#pragma once

#include "BackwardIntegrator.h"

class NormalDebugIntegrator : public BackwardIntegrator {
public:
    NormalDebugIntegrator() = delete;
    using BackwardIntegrator::BackwardIntegrator; //Inherit the base class constructor

    //Tecnique derived from https://graphics.pixar.com/library/RendermanTog2018/paper.pdf (Integrators for simple rendering)
    //We just need to sample the scene normals
    Color sample(const Ray &r) const override {

        //If there are no intersections call the no hit shader
        ObjectIntersection intersection;
        if (!getClosestSceneIntersection(r, intersection)) {
            return noIntersections(r);
        }

        return (Point(1.0) + intersection.pn) * Point(0.5f);
    }

};
