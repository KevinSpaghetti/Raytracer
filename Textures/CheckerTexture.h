//
// Created by kevin on 07/09/20.
//

#pragma once

#include <cmath>
#include "Texture.h"

class CheckerTexture : public Texture {
public:
    CheckerTexture(): a({0.0, 0.0, 0.0}), b({1.0, 1.0, 1.0}) {}

    CheckerTexture(Color a, Color b) : a(a), b(b) {}

    Color value(UV at) override {
        auto sines = sin(10*at.s)*sin(10*at.t);
        if (sines < 0)
            return a;
        else
            return b;
    }

private:
    Color a;
    Color b;

};
