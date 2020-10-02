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
            if(intersections::ray_plane(r, center, -normal, t, isFront)){
                Point ip = r.at(t);
                if(ip.x < center.x - width/2.0f || ip.x > center.x + width/2.0f){
                    return ;
                }
                if(ip.z < center.z - height/2.0f || ip.z > center.z + height/2.0f){
                    return ;
                }
                //TODO: implement uvs
                intersections.push_back(Intersection{ip, normal, {0, 0, 0}, isFront});
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

    virtual double pdf(const Point& origin, const Normal& direction) const {
        std::vector<Intersection> ins;
        this->intersect(Ray(origin, direction), ins);
        if (ins.empty()) return 0.0f;

        Intersection i = ins.front();

        //Doesn't work because we need to transform the light to world space
        float area = width * height;
        float distance_squared = glm::length(i.pv - origin) * glm::length(i.pv - origin) * glm::length(direction);
        float cosine = fabs(glm::dot(direction, i.pn) / glm::length(direction));

        return distance_squared / (cosine * area);
    }

    virtual Point random(const Point& o) const {
        auto random_point = Point{randomized::scalar::random(-width/2, width/2),
                             0,
                             randomized::scalar::random(-height/2, height/2)};
        return center + random_point;
    }

private:
    Axis type;
    Point center;
    float width;
    float height;
};
