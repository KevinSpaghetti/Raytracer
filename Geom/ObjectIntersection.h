//
// Created by kevin on 07/09/20.
//

#pragma once

#include "Intersection.h"

class Node;

struct ObjectIntersection : Intersection {
    //The scene graph node that contains the hit object
    std::shared_ptr<Node> node;
};
