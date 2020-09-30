//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Mesh.h"

//Class to represent an Axis Aligned rect
class AARectMesh : public Mesh {
public:

    enum Axis{
        XZ,
        YZ, //Not implemented
        XY  //Not implemented
    };

    AARectMesh(const Axis type, const Point center, const float width, const float height) :
            type(type),
            center(center),
            width(width),
            height(height) {}

    void intersect(const Ray &r, std::vector<Intersection>& intersections) const override {
        if(type == XZ){
            float t;
            bool isFront;
            const Normal normal = {0, -1, 0};
            if(intersections::ray_plane(r, center, normal, t, isFront)){
                Point ip = r.at(t);
                if(ip.x < center.x - width/2.0f || ip.x > center.x + width/2.0f){
                    return ;
                }
                if(ip.z < center.z - height/2.0f || ip.z > center.z + height/2.0f){
                    return ;
                }
                if(glm::dot(normal, r.getDirection()) > 0.0f){
                    //TODO: implement uvs
                    intersections.push_back({ip, normal, {0, 0, 0}, isFront});
                }else{
                    intersections.push_back({ip, normal, {0, 0, 0}, isFront});
                }
            }
        }else{
            return ;
        }
    }

    AABB getSurroundingBox() const override {
        if(type == XZ){
            const Point min = center - Point{width/2.0f, 0.0001, height/2.0f};
            const Point max = center + Point{width/2.0f, 0.0001, height/2.0f};
            return AABB(min, max);
        }

        return AABB(Point(0), Point(0));
    }

private:
    Axis type;
    Point center;
    float width;
    float height;
};