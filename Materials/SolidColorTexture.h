//
// Created by kevin on 07/09/20.
//

#pragma once

#include "Texture.h"

class SolidColorTexture : public Texture {
public:
    SolidColorTexture(Color c) : color(c) {}

    Color value(UV at) override {
        return color;
    }

private:
    Color color;
};