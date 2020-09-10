//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Boxable.h"
#include "VertexBasedShape.h"

class AABB : public BoundingBox {
public:
    //Default constructor is a bounding box which never gets hit
    //Better than -inf, +inf bounding box because we can start
    //with the empty bounding box then expand
    AABB() : min({0,0,0}), max({0,0,0}) {}

    AABB(Point min, Point max) : min(min), max(max) {}

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

    AABB(std::shared_ptr<VertexBasedShape> shape){
        min = {0, 0, 0};
        max = {0, 0, 0};
        //Grow the bounding box
        for(Vertex v : shape->verticesAsArray()){
            for (int i = 0; i < 3; ++i) {
                min[i] = std::min(min[i], v[i]);
            }
            for (int i = 0; i < 3; ++i) {
                max[i] = std::min(max[i], v[i]);
            }
        }

    }

    bool isHit(const Ray& r) override {
        for (int a = 0; a < 3; a++) {
            float t0 = fmin((min[a] - r.getOrigin()[a]) / r.getDirection()[a],
                           (max[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            float t1 = fmax((min[a] - r.getOrigin()[a]) / r.getDirection()[a],
                           (max[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            float tmin = fmax(t0, r.getTmin());
            float tmax = fmin(t1, r.getTmax());
            if (tmax <= tmin)
                return false; //No intersections
        }
        return true;
    }

    Point getMin() override {
        return min;
    }
    Point getMax() override {
        return max;
    }

private:
    Point min;
    Point max;
};