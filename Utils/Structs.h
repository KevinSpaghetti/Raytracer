//
// Created by kevin on 17/08/20.
//

#pragma once

#include <glm/vec3.hpp>

typedef glm::vec3 Vertex;
typedef glm::vec3 Point;

typedef glm::vec3 Normal;

typedef glm::vec3 UV;

struct Triangle {
    //Vertex indices
    int vta;
    int vtb;
    int vtc;
    //Vertex uvs indices
    int uva;
    int uvb;
    int uvc;
    //Vertex normals indices
    int nma;
    int nmb;
    int nmc;
};

typedef glm::vec3 Color;
