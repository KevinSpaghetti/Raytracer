//
// Created by kevin on 07/09/20.
//

#pragma once

#include <functional>
#include "Shader.h"
#include "../Utils/Structs.h"
#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"

class HitShader : public Shader {
public:
    HitShader(std::function<Color(Ray, Intersection)> evaluate,
              std::function<Ray(Ray, Intersection)> scatter) : code(evaluate), scattered(scatter) {}

    Ray scatter(Ray r, Intersection i){
        return scattered(r, i);
    }

    Color color(Ray r, Intersection i){
        return code(r, i);
    }

private:
    std::function<Ray(Ray, Intersection)> scattered;
    std::function<Color(Ray, Intersection)> code;
};
