//
// Created by kevin on 15/10/20.
//

#pragma once

#include "LightNode.h"
#include "../Utils/Structs.h"
#include "../Mesh/SphereMesh.h"

class SphereLightNode : public LightNode {
public:
    SphereLightNode(const float radius, const Color color, const float intensity)
            : LightNode(std::make_shared<SphereMesh>(radius),
                        std::make_shared<DiffuseLight>(color, intensity)),
              radius(radius),
              center({0, 0, 0}) {}

    Ray randomPoint(const Point& origin) const override {
        auto random_point = randomized::vector::in_unit_sphere() * radius;
        random_point = global.pointToWorldSpace(random_point);
        //Set the ray as a shadow ray
        return Ray(origin, glm::normalize(random_point - origin), Ray::Type::Shadow);
    }


protected:
    float radius;

    Point center; //center for our local space
};
