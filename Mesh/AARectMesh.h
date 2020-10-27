//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Mesh.h"

//TODO: test uvs

//Class to represent an Axis Aligned rect
class AARectMesh : public Mesh {
public:

    enum Axis{
        XZ
    };

    AARectMesh(const Axis type, const Point center, const Normal normal, const float width, const float height) :
            type(type),
            center(center),
            normal(normal),
            width(width),
            height(height) {}

    void intersect(const Ray& r, std::array<Intersection, 2>& intersections, int& n_intersections) const override {
        if(type == XZ){
            float t;
            bool isFront;
            if(intersections::ray_plane(r, center, normal, t, isFront)){
                Point ip = r.at(t);
                if(ip.x < center.x - width/2.0f || ip.x > center.x + width/2.0f){
                    return ;
                }
                if(ip.z < center.z - height/2.0f || ip.z > center.z + height/2.0f){
                    return ;
                }
                Point upper_left{center.x - width/2.0f, 0.0f, center.y - height/2.0f};
                Point on_surface{ip - upper_left};
                UV uvs = {on_surface.x / width, on_surface.z / height, 0.0f};
                uvs.y = 1.0f - uvs.y;
                n_intersections = 1;
                intersections[0] = {ip, normal, t, uvs, isFront};
            }
        }else{
            return ;
        }
    }

    AABB getSurroundingBox() const override {
        if(type == XZ){
            const Point min = center - Point{width/2.0f, 0.001, height/2.0f};
            const Point max = center + Point{width/2.0f, 0.001, height/2.0f};
            return AABB(min, max);
        }

        return AABB(Point(0), Point(0));
    }


private:
    Axis type;
    Point center;
    Point normal;
    float width;
    float height;

};
