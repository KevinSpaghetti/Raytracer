//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Boxable.h"
#include "VertexBasedShape.h"
#include "../SceneGraph/IntersectTestable.h"

class AABB : public BoundingBox, public IntersectTestable {
public:
    //Default constructor is a bounding box which never gets hit
    //Better than -inf, +inf bounding box because we can start
    //with the empty bounding box then expand
    AABB() : min({0,0,0}), max({0,0,0}) {}

    AABB(Point min, Point max) : min(min), max(max) {}

    //Create a bounding box which encloses all the points in the list
    AABB(const std::vector<Point>& points){
        min = {0, 0, 0};
        max = {0, 0, 0};
        //Grow the bounding box
        for(Vertex v : points){
            for (int i = 0; i < 3; ++i) {
                min[i] = std::min(min[i], v[i]);
            }
            for (int i = 0; i < 3; ++i) {
                max[i] = std::max(max[i], v[i]);
            }
        }
    }

    //Construct a bounding box from the two passed
    AABB(std::shared_ptr<BoundingBox> a, std::shared_ptr<BoundingBox> b){
        vec3 small(fmin(a->getMin().x, b->getMin().x),
                   fmin(a->getMin().y, b->getMin().y),
                   fmin(a->getMin().z, b->getMin().z));
        vec3 big(fmax(a->getMax().x, b->getMax().x),
                 fmax(a->getMax().y, b->getMax().y),
                 fmax(a->getMax().z, b->getMax().z));
        min = small;
        max = big;
    }

    AABB(const std::shared_ptr<VertexBasedShape>& shape) : AABB(shape->verticesAsArray()){}

    bool isHit(const Ray& r) const override {
        float tmin, tmax;
        return getHitPoint(r, tmin, tmax);
    }
    //One intersection enters and one exits the cube
    bool getHitPoint(const Ray& r, float& tmin, float& tmax) const {
        for (int a = 0; a < 3; a++) {
            float t0 = fmin((min[a] - r.getOrigin()[a]) / r.getDirection()[a],
                            (max[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            float t1 = fmax((min[a] - r.getOrigin()[a]) / r.getDirection()[a],
                            (max[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            tmin = fmax(t0, r.getTmin());
            tmax = fmin(t1, r.getTmax());
            if (tmax <= tmin)
                return false; //No intersections
        }
        return true;
    }

    Point getMin() const override {
        return min;
    }
    Point getMax() const override {
        return max;
    }

    std::list<Intersection> intersect(const Ray &r) const override {
        std::list<Intersection> ins;
        float tmin, tmax;
        if(getHitPoint(r, tmin, tmax)){
            Point ip = r.getOrigin() + r.getDirection() * tmin;
            Point box_center = max + ((max - min)/2.0f);
            Normal nm = box_center - ip;
            ins.push_back({
                ip, //Intersection point
                glm::normalize(nm),
                {0, 0, 0}
            });
            ip = r.getOrigin() + r.getDirection() * tmax;
            nm = box_center - ip;
            ins.push_back({
                ip, //Intersection point
                glm::normalize(nm),
                {0, 0, 0}
            });
        }
        return ins;
    }

private:


    Point min;
    Point max;
};