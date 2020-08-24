//
// Created by kevin on 17/08/20.
//

#pragma once

#include <glm/vec3.hpp>

struct Vertex {
    float x;
    float y;
    float z;

    glm::vec3 vec(){
        return glm::vec3(x, y, z);
    }
};

struct Normal {
    float i;
    float j;
    float k;
    glm::vec3 vec(){
        return glm::vec3(i, j, k);
    }
};

struct UV {
    float u;
    float v;
    float w;
};

struct Triangle {
    int a;
    int b;
    int c;
};

struct Color {
    float r;
    float g;
    float b;
};
