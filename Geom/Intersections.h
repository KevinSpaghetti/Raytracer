//
// Created by kevin on 24/08/20.
//

#pragma once

#include <optional>
#include "Intersection.h"


//Intersection tests must respect the min and max boundaries of the ray
namespace intersections {
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/geometry-of-a-triangle
    //Ray Triangle
    bool ray_triangle(const Ray& r, const Vertex& v1, const Vertex& v2, const Vertex& v3, float& t, float& u, float& v, bool& isFrontFace) {

        glm::vec3 A = v2 - v1;
        glm::vec3 B = v3 - v1;
        glm::vec3 pvec = glm::cross(r.getDirection(), B);
        float det = glm::dot(A, pvec);

        if(det < consts::epsilon) isFrontFace = false; //Backface culling

        if(fabs(det) < consts::epsilon) return false;

        float invDet = 1 / det;
        glm::vec3 tvec = r.getOrigin() - v1;
        u = glm::dot(tvec, pvec) * invDet;
        if(u < 0 || u > 1) return false;

        glm::vec3 qvec = glm::cross(tvec, A);
        v = glm::dot(r.getDirection(), qvec) * invDet;
        if(v < 0 || u + v > 1) return false;

        t = glm::dot(B, qvec) * invDet;

        //Discard intersections before and after the ray
        if(t < r.getTmin() || t > r.getTmax()) {
            return false;
        }
        return true;
    }

    //Ray Plane
    bool ray_plane(const Ray& r, const Point& center, const Normal& normal, float& t, bool& isFrontFace){
        //check if the point lies on the plane
        float angle = glm::dot(normal, r.getDirection());
        if(abs(angle) > 0.00001){
            Point p0l0 = center - r.getOrigin();
            t = glm::dot(p0l0, normal) / angle;
            if (t < r.getTmin() || t > r.getTmax()){
                return false;
            }
            if(glm::dot(r.getDirection(), normal) > 0){
                isFrontFace = false;
            }else{
                isFrontFace = true;
            }
            return true;
        }
        return false;
    }

    //Ray Rect
    bool ray_rect(const Ray& r, const Point& center, const Normal& normal, const float& width, const float& height, float& t, bool& isFrontFace){
        //check if the point lies on the plane
        float denom = glm::dot(-normal, r.getDirection());
        if(denom > 1e-6){
            Point p0l0 = center - r.getOrigin();
            t = glm::dot(p0l0, -normal) / denom;
            if (t < 0) {
                return false;
            }
            if (t < r.getTmin() || t > r.getTmax()){
                return false;
            }
            Point surfacepoint = r.at(t);
            Point origin = center - Point{width/2.0f, height/2.0f, 0.0};
            Point ds = surfacepoint - center;

            //Does Not Work for oriented rects
            //Check if the point lies inside the area of the rect
            if(ds.x < origin.x) return false;
            if(ds.x > (origin + width).x) return false;
            if(ds.y < origin.y) return false;
            if(ds.y > (origin + height).y) return false;

            if(glm::dot(r.getDirection(), -normal) > 0){
                isFrontFace = false;
            }else{
                isFrontFace = true;
            }
            return true;
        }
        return false;
    }

    //Ray Disk
    bool ray_disk(const Ray& r, const Point& center, const Normal& normal, const float& radius, float& t, bool& isFrontFace) {
        //check if the point lies on the plane
        float denom = glm::dot(-normal, r.getDirection());
        if (denom > 1e-6) {
            Point p0l0 = center - r.getOrigin();
            t = glm::dot(p0l0, -normal) / denom;
            if (t < 0) {
                return false;
            }
            if (t < r.getTmin() || t > r.getTmax()) {
                return false;
            }

            Point surfacepoint = r.at(t);
            Point v = surfacepoint - center;
            float d = glm::dot(v, v);
            return sqrtf(d) <= radius;
        }
        return false;
    }

}