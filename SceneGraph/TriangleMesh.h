//
// Created by kevin on 04/09/20.
//

#pragma once

#include "Mesh.h"

class TriangleMesh : public Mesh {
public:
    TriangleMesh() : vertices(),
             triangles(),
             normals(),
             uvs() {}

    TriangleMesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, std::vector<Normal>& normals, std::vector<UV>& uvs) :
    vertices(vertices),
    triangles(triangles),
    normals(normals),
    uvs(uvs){}

    std::list<Intersection> intersect(const Ray& r) const override {
        std::list<Intersection> ins;
        for (Triangle tri : triangles) {
            Vertex v1 = vertices[tri.a - 1];
            Vertex v2 = vertices[tri.b - 1];
            Vertex v3 = vertices[tri.c - 1];

            float t, u, v;
            if(intersections::intersect(r, v1, v2, v3, t, u, v)){
                //Interpolate the informations from all the vertices
                Vertex ip{0, 0, 0};
                ip = u * v1 + v * v2 + (1-u-v) * v3;
                Normal nm{0, 0, 0};
                UV uv{0, 0, 0};
                //Check if the normals are there
                if(hasNormals()){
                    Normal n1 = normals[tri.a - 1];
                    Normal n2 = normals[tri.b - 1];
                    Normal n3 = normals[tri.c - 1];
                    nm = u * n1 + v * n2 + (1-u-v) * n3;
                }

                if(hasUVs()){
                    UV uv1 = uvs[tri.a - 1];
                    UV uv2 = uvs[tri.b - 1];
                    UV uv3 = uvs[tri.c - 1];
                    uv = u * uv1 + v * uv2 + (1-u-v) * uv3;
                    uv.y = (1.0 - uv.y); //Invert the y axis
                }

                ins.push_back({ip, nm, uv});

            }
        }

        return ins;
    }

    bool hasNormals() const {
        return !normals.empty();
    }
    bool hasUVs() const {
        return !uvs.empty();
    }
    bool hasVertices() const {
        return !vertices.empty();
    }
    bool hasTriangles() const {
        return !triangles.empty();
    }

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;
};
