//
// Created by kevin on 15/10/20.
//

#pragma once

#include "VisualNode.h"
#include "../Materials/Material.h"

class LightNode : public VisualNode {
protected:
    LightNode(std::shared_ptr<Mesh>&& mesh, std::shared_ptr<Material>&& material) :
        VisualNode(std::forward<std::shared_ptr<Mesh>>(mesh), std::shared_ptr<Material>(material)) {}

public:
    Type type() const override {
        return Type::Light;
    }

};
