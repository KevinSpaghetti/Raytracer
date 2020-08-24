//
// Created by kevin on 24/08/20.
//

#pragma once

#include "Ray.h"
#include "../SceneGraph/Node.h"
#include "../Utils/Consts.h"


struct Intersection {
    Vertex pv; //Point in which the ray hits the triangle
    Normal pn; //Normal of the point in which the ray hits the triangle
};