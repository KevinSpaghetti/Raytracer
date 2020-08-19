//
// Created by kevin on 15/08/20.
//
#pragma once

#include <memory>
#include <iostream>
#include <list>
#include "Geometry.h"
#include "../Materials/Material.h"
#include "Transform.h"

class Node : public Transform {
public:
    Node(){}

    Node(const std::shared_ptr<Geometry> geometry, const std::shared_ptr<Material> material) : geometry(geometry), material(material) {}

    void add(const Node& child){
        children->push_back(child);
    }

protected:
    std::shared_ptr<Geometry> geometry;
    std::shared_ptr<Material> material;
    std::shared_ptr<std::list<Node>> children;
};