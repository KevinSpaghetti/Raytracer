//
// Created by kevin on 31/08/20.
//

#pragma once

#include "../Utils/Structs.h"


class Texture {
public:
    //Returned values must be in the 0-1 range
    virtual Color value(UV at) = 0;
};

