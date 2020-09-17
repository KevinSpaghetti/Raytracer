//
// Created by kevin on 24/08/20.
//

#pragma once

#include "Ray.h"
#include "../SceneGraph/Node.h"
#include "../Utils/Consts.h"
#include "../Materials/Material.h"
#include "../Utils/Structs.h"

struct Intersection {
    Vertex pv; //Point in which the ray hits the mesh
    Normal pn; //Normal of the point in which the ray hits the mesh
    UV uv;

    //Backface culling options
    bool isFront = true;
};
