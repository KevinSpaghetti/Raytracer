//
// Created by kevin on 07/09/20.
//

#pragma once

#include <functional>
#include "NoHitShader.h"
#include "../Geom/Ray.h"
#include "../Utils/Structs.h"

class MaxDepthShader : public Shader {
public:
    MaxDepthShader(std::function<Color(Ray)> evaluate) : code(evaluate) {}

    Color color(Ray r){
        return code(r);
    }

private:
    std::function<Color(Ray)> code;
};