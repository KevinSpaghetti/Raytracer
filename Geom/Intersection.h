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
    //TODO: Add point normal in object and world space
    //Object Space
    Vertex point{}; //Point in which the ray hits the mesh
    Normal normal{}; //Normal of the point in which the ray hits the mesh

    //Both for object and for world space
    UV uv{};

    bool isFront = true;

    //World space can be computed only when creating an object
    //intersection since the mesh does not hold any transforms,
    //its the node that transforms the intersection object space
    //coordinates (point, normal) into world space

    //World space
    Vertex ws_point{};
    Vertex ws_normal{};
};
