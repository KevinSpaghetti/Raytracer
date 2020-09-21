//
// Created by Kevin on 12/09/2020.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"
#include "../Utils/Triangle.h"
#include "../Geom/Intersection.h"

//Struct that implements the triangle mesh data
//allows to pass around data easily
struct TriangleMeshData {

    TriangleMeshData(std::vector<Vertex> vertices, std::vector<Triangle> triangles, std::vector<Normal> normals, std::vector<UV> uvs) : vertices(vertices),
            triangles(triangles),
            normals(normals),
            uvs(uvs) {}

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;

    bool test(const Ray& r, const Triangle t, Intersection& i){
        Vertex v1 = vertices[t.vta];
        Vertex v2 = vertices[t.vtb];
        Vertex v3 = vertices[t.vtc];

        float d;
        float u, v;
        bool isFrontFace;
        if(intersections::intersect(r, v1, v2, v3, d, u, v, isFrontFace)){
            //TODO: Check the normalization of all the directions
            //Interpolate the informations from all the vertices
            Vertex ip{0, 0, 0};
            Normal nm{0, 0, 0};
            UV uv{0, 0, 0};

            //Worse method, see raytracing gems 3
            //Gets progressively worse with more camera distance
            //ip = r.getOrigin() + glm::normalize(r.getDirection()) * t;
            ip = (1.0f-u-v) * v1 + u * v2 + v * v3; //Does not depend on the direction normalization
            //Check if the normals and uv are present before adding
            if(!normals.empty()){
                Normal n1 = normals[t.nma];
                Normal n2 = normals[t.nmb];
                Normal n3 = normals[t.nmc];
                nm = (1.0f-u-v) * n1 + u * n2 + v * n3;
            }
            if(!uvs.empty()){
                UV uv1 = uvs[t.uva];
                UV uv2 = uvs[t.uvb];
                UV uv3 = uvs[t.uvc];
                uv = (1.0f-u-v) * uv1 + u * uv2 + v * uv3;
                uv.y = (1.0 - uv.y); //Invert the y axis
            }

            i = Intersection{
                ip,
                nm,
                uv,
                isFrontFace
            };
            return true;
        }
        return false;
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
};