//
// Created by kevin on 26/08/20.
//

#pragma once

#include "Material.h"
#include "../Geom/Ray.h"
#include "../consts.h"

class VNMaterial : public Material {
    glm::vec3 scatter(const Ray& r, const Intersection& i) const {
        return (glm::normalize(i.pn + glm::vec3(1, 1, 1)));
    }

};