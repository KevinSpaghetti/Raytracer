//
// Created by kevin on 10/09/20.
//

#pragma once

#include <vector>
#include "../Utils/Structs.h"

class VertexBasedShape {
public:

    virtual std::vector<Vertex> verticesAsArray() = 0;


};