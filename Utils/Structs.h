//
// Created by kevin on 17/08/20.
//

#pragma once

struct Vertex {
    float x;
    float y;
    float z;
};

struct Normal {
    float i;
    float j;
    float k;
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
