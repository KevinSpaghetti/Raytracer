//
// Created by kevin on 01/10/20.
//

#pragma once

#include "Mesh.h"

class BoxMesh : public Mesh {
public:

    BoxMesh(Point min, Point max) : min(min), max(max) {}

    virtual void intersect(const Ray& r, std::vector<Intersection>& intersections) const override {

        float tmin, tmax;
        if(getHitPoint(r, tmin, tmax)){
            Point ip = r.getOrigin() + r.getDirection() * tmin;
            Point box_center = max + ((max - min)/2.0f);
            Normal nm = glm::normalize(ip - box_center);
            intersections.push_back(Intersection{ip, nm, {0, 0, 0}, true});
            ip = r.getOrigin() + r.getDirection() * tmax;
            nm = glm::normalize(ip - box_center);
            intersections.push_back(Intersection{ip, nm, {0, 0, 0}, false});
        }
    }

    AABB getSurroundingBox() const override {
        return Mesh::getSurroundingBox();
    }

private:
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

        if(tmin < r.getTmin() || tmax < r.getTmax() ){
            return false;
        }

        return true;
    }

    Point min;
    Point max;
};