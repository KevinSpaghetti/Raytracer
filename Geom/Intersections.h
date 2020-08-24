//
// Created by kevin on 24/08/20.
//

#pragma once

#include <optional>
#include "Intersection.h"



namespace intersections {
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/geometry-of-a-triangle

    bool intersect(const Ray& r, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float& u, float& v, float& t) {
        const float kEpsilon = 0.000001;

        glm::vec3 A = v2 - v1;
        glm::vec3 B = v3 - v1;
        glm::vec3 C = glm::cross(r.getDirection(), B);
        float det = glm::dot(A, C);
        //if(det < kEpsilon) backface
        //(fabs(det) < kEpsilon) parrallel to triangle
        float invDet = 1 / det;
        glm::vec3 tvec = r.getOrigin() - v1;
        u = glm::dot(tvec, A);
        if (u < 0 || u > 1) return false; //No intersection
        glm::vec3 qvec = glm::cross(tvec, A);
        v = glm::dot(r.getDirection(), qvec) * invDet;
        if (v < 0 || u + v > 1) return false; //No intersection

        t = glm::dot(B, qvec) * invDet;
        return true;
    }


}