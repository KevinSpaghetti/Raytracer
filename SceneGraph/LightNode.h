//
// Created by kevin on 15/10/20.
//

#pragma once

#include "VisualNode.h"
#include "../Materials/Material.h"

class LightNode : public VisualNode {
protected:
    LightNode(const std::shared_ptr<Mesh> mesh,const std::shared_ptr<Material> material) : VisualNode(mesh, material) {}

public:
    Type type() const override {
        return Type::Light;
    }



};
