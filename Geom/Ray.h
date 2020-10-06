//
// Created by kevin on 12/08/20.
//
#pragma once

#include <glm/glm.hpp>
#include "../Utils/Consts.h"

class Ray {
public:
    enum class Type {
        Camera,
        Diffuse,
        Specular,
        Transmission,
        Shadow,
        Unknown
    };

    //Init the ray with some values so we don't get errors for uninitialized memory
    Ray() :
        type(Type::Unknown),
        origin({0, 0, 0}),
        direction({0, 0, 0}), //Dangerous when normalizing causes crash
        tmin(0.001),
        tmax(+consts::infinity) {}
    Ray(const Point origin, const Normal direction, const Type type = Type::Unknown, const float tmin = 0.0001, const float tmax = consts::infinity) :
        type(type),
        origin(origin),
        direction(glm::normalize(direction)),
        tmin(tmin),
        tmax(tmax) {}

    Point getOrigin() const {
        return origin;
    }
    Normal getDirection() const {
        return direction;
    }

    Point at(float distance) const {
        return origin + (direction * distance);
    }

    float getTmin() const {
        return tmin;
    }
    float getTmax() const {
        return tmax;
    }
    Type getType() const{
        return type;
    }

private:
    Type type;
    Point origin;
    Normal direction;

    float tmin;
    float tmax;
};
