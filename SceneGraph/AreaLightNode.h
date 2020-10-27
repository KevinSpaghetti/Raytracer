//
// Created by kevin on 15/10/20.
//

#pragma once

#include "LightNode.h"
#include "../Mesh/AARectMesh.h"

class AreaLightNode : public LightNode {
public:
    //Area light node is a special type of visual node used in importance sampling
    AreaLightNode(const float width, const float height, const Color color, const float intensity)
            : LightNode(std::make_shared<AARectMesh>(AARectMesh::Axis::XZ, Point{0, 0, 0}, Normal{0, -1, 0}, width, height),
                        std::make_shared<DiffuseLight>(color, intensity)),
              width(width),
              height(height),
              center({0, 0, 0}) {}

protected:
    float width;
    float height;
    Point center;
};
