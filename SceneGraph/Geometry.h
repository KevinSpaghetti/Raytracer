//
// Created by kevin on 15/08/20.
//

#pragma once

#include <utility>
#include <vector>
#include "../Utils/Structs.h"
#include "Transform.h"

class Geometry {
public:
    Geometry() {}

    Geometry(std::vector<Vertex>& vertices,
             std::vector<Triangle>& triangles,
             std::vector<Normal>& normals,
             std::vector<UV>& uvs)
        : vertices(vertices),
          triangles(triangles),
          normals(normals),
          uvs(uvs){}

    //Apply the transform to all the vertices
    //And normals
    void apply(Transform t){
        for (int i = 0; i < vertices.size(); ++i) {
            glm::vec4 v(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
            glm::vec3 a = t.apply(v);
            vertices[i] = Vertex{a.x, a.y, a.z};
        }
        for (int i = 0; i < normals.size(); ++i) {
            glm::vec4 v(normals[i].i, normals[i].j, normals[i].k, 0.0f);
            glm::vec3 a = t.apply(v);
            normals[i] = Normal{a.x, a.y, a.z};
        }
    }


public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;

};
