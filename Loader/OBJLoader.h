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
#include "../Mesh/TriangleMesh.h"

class OBJLoader : public Loader<TriangleMesh> {
public:
    OBJLoader() {}

    //Check if the indexing is 1 based or 0 based
    TriangleMesh load(std::string filename) const override {
        std::string full_path = filename;

        std::vector<Vertex> vertices;
        //TODO: Add the support to faces with vertices with different 1/2/3
        //v, vt, vn
        //TODO: Add the support for groups etc
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
            //f 1/1/1 2/2/2 3/3/3
            //f v/vt/vn v/vt/vn v/vt/vn
            if (type == "f"){
                Triangle tv{}; //Triangle vertices
                int t_vt[3];
                int t_uv[3];
                int t_nm[3];
                for (int i = 0; i < 3; ++i) {
                    char c;
                    tp >> t_vt[i];
                    tp >> c;
                    tp >> t_uv[i];
                    tp >> c;
                    tp >> t_nm[i];
                }
                tv.vta = t_vt[0] - 1;
                tv.vtb = t_vt[1] - 1;
                tv.vtc = t_vt[2] - 1;
                tv.uva = t_uv[0] - 1;
                tv.uvb = t_uv[1] - 1;
                tv.uvc = t_uv[2] - 1;
                tv.nma = t_nm[0] - 1;
                tv.nmb = t_nm[1] - 1;
                tv.nmc = t_nm[2] - 1;

                triangles.push_back(tv);
            }
            if (type == "g"){
                std::string name;
                tp >> name;
            }
        }

        file.close();

        return TriangleMesh(std::move(vertices), std::move(triangles), std::move(normals), std::move(uvs));
    }
};