//
// Created by kevin on 17/08/20.
//

#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include "Loader.h"
#include "../SceneGraph/Geometry.h"
#include "../SceneGraph/TriangleMesh.h"

class OBJLoader : public Loader<TriangleMesh> {
public:
    OBJLoader() {}

    TriangleMesh load(std::string filename) const override {
        std::string full_path = filename;

        std::vector<Vertex> vertices;
        std::vector<Triangle> triangles;
        std::vector<Normal> normals;
        std::vector<UV> uvs;

        std::ifstream file;
        file.open(full_path);
        std::string line;
        while(getline(file, line)) {
            std::istringstream tp(line);
            std::string type;
            tp >> type;
            if (type == "v"){
                Vertex v{};
                tp >> v.x;
                tp >> v.y;
                tp >> v.z;
                vertices.push_back(v);
            }
            if (type == "vn"){
                Normal vn{};
                tp >> vn.x;
                tp >> vn.y;
                tp >> vn.z;
                normals.push_back(vn);
            }
            if (type == "vt"){
                UV vt{};
                tp >> vt.s;
                tp >> vt.t;
                tp >> vt.p;
                uvs.push_back(vt);
            }
            if (type == "f"){
                Triangle t{};
                tp >> t.a;
                tp >> t.b;
                tp >> t.c;
                triangles.push_back(t);
            }
        }

        file.close();

        return TriangleMesh(vertices, triangles, normals, uvs);
    }
};