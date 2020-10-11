//
// Created by kevin on 04/09/20.
//

#pragma once

#include <algorithm>
#include "Boxable.h"
#include "../Geom/IntersectTestable.h"
#include "BoundingBox.h"

class AABB : public BoundingBox {
public:
    //Default constructor is a bounding box which never gets hit
    //Better than -inf, +inf bounding box because we can start
    //with the empty bounding box then expand
    AABB() : min(Point{+consts::infinity}), max(Point{-consts::infinity}) {}
    AABB(Point min, Point max) : min(min), max(max) {}
    //Create a bounding box which encloses all the points in the list
    AABB(const std::vector<Point>& points){
        min = {+consts::infinity, +consts::infinity, +consts::infinity};
        max = -min;
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
    AABB(AABB a, AABB b){
        Point small(fmin(a.getMin().x, b.getMin().x),
                   fmin(a.getMin().y, b.getMin().y),
                   fmin(a.getMin().z, b.getMin().z));
        Point big(fmax(a.getMax().x, b.getMax().x),
                 fmax(a.getMax().y, b.getMax().y),
                 fmax(a.getMax().z, b.getMax().z));
        min = small;
        max = big;
    }

    //Grow this bbox
    void grow(AABB b){
        Point small(fmin(min.x, b.getMin().x),
                    fmin(min.y, b.getMin().y),
                    fmin(min.z, b.getMin().z));
        Point big(fmax(max.x, b.getMax().x),
                  fmax(max.y, b.getMax().y),
                  fmax(max.z, b.getMax().z));
        min = small;
        max = big;
    }

    bool isHit(const Ray& r) const override {
        float tmin = 0.0f;
        float tmax = 0.0f;
        return getHitPoint(r, tmin, tmax);
    }
    //One intersection enters and one exits the cube
    bool getHitPoint(const Ray& r, float& tmin, float& tmax) const {

        tmin = (min.x - r.getOrigin().x) / r.getDirection().x;
        tmax = (max.x - r.getOrigin().x) / r.getDirection().x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (min.y - r.getOrigin().y) / r.getDirection().y;
        float tymax = (max.y - r.getOrigin().y) / r.getDirection().y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (min.z - r.getOrigin().z) / r.getDirection().z;
        float tzmax = (max.z - r.getOrigin().z) / r.getDirection().z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }

    Point getMin() const override {
        return min;
    }
    Point getMax() const override {
        return max;
    }

    Point getCenter() const {
        return min + (max - min) / 2.0f;
    }

private:
    Point min;
    Point max;
};

AABB applyTransform(AABB box, GlobalTransform t){
    Dimension extent = box.getMax() - box.getMin();
    Point min = box.getMin();
    Point max = box.getMax();
    std::vector<Point> points({
              t.pointToWorldSpace(min),
              t.pointToWorldSpace(min + Point{extent.x, 0.0f, 0.0f}),
              t.pointToWorldSpace(min + Point{0.0f, extent.y, 0.0f}),
              t.pointToWorldSpace(min + Point{0.0f, 0.0f, extent.z}),
              t.pointToWorldSpace(max - Point{extent.x, 0.0f, 0.0f}),
              t.pointToWorldSpace(max - Point{0.0f, extent.y, 0.0f}),
              t.pointToWorldSpace(max - Point{0.0f, 0.0f, extent.z}),
              t.pointToWorldSpace(max)
      });
    return AABB(points);
}
