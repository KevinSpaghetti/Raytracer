//
// Created by kevin on 15/08/20.
//

#pragma once

#include <utility>
#include <vector>
#include "../Utils/Structs.h"

class Geometry {
public:
    Geometry(std::vector<Vertex>& vertices,
             std::vector<Triangle>& triangles,
             std::vector<Normal>& normals,
             std::vector<UV>& uvs)
        : vertices(vertices),
          triangles(triangles),
          normals(normals),
          uvs(uvs){}


private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;

};
