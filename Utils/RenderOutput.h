//
// Created by Kevin on 18/08/2020.
//

#pragma once

#include "Buffer.h"

class RenderOutput {
public:
    virtual void write(ColorBufferFormat& buffer) const = 0;

};
