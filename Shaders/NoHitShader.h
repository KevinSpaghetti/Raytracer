//
// Created by kevin on 07/09/20.
//

#pragma once

#include "Shader.h"

class NoHitShader : public Shader {
public:
    NoHitShader(std::function<Color(Ray)> evaluate) : code(evaluate) {}

    Color color(Ray r){
        return code(r);
    }

private:
    std::function<Color(Ray)> code;
};