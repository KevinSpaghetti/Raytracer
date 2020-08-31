//
// Created by kevin on 31/08/20.
//

#pragma once

#include "../Utils/Structs.h"

class Texture {
public:
    virtual Color value(UV at) = 0;
};

