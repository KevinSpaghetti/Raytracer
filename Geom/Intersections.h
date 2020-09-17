//
// Created by kevin on 24/08/20.
//

#pragma once

#include <optional>
#include "Intersection.h"


//Intersection tests must respect the min and max boundaries of the ray
namespace intersections {
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/geometry-of-a-triangle

    bool intersect(const Ray& r, const Vertex& v1, const Vertex& v2, const Vertex& v3, float& t, float& u, float& v, bool& isFrontFace) {

        glm::vec3 A = v2 - v1;
        glm::vec3 B = v3 - v1;
        glm::vec3 pvec = glm::cross(glm::normalize(r.getDirection()), B);
        float det = glm::dot(A, pvec);

        if(det < consts::epsilon) isFrontFace = false; //Backface culling

        if(fabs(det) < consts::epsilon) return false;

        float invDet = 1 / det;
        glm::vec3 tvec = r.getOrigin() - v1;
        u = glm::dot(tvec, pvec) * invDet;
        if(u < 0 || u > 1) return false;

        glm::vec3 qvec = glm::cross(tvec, A);
        v = glm::dot(glm::normalize(r.getDirection()), qvec) * invDet;
        if(v < 0 || u + v > 1) return false;

        t = glm::dot(B, qvec) * invDet;

        //Discard intersections before and after the ray
        if(t < r.getTmin() || t > r.getTmax()) {
            return false;
        }


        return true;
    }


}