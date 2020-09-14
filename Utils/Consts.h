//
// Created by kevin on 15/08/20.
//

#pragma once

#include <limits>
#include "Structs.h"

namespace consts {

    const float infinity = std::numeric_limits<float>::infinity();
    const float pi = 3.1415926535897932385;
    const float epsilon = 0.000001;

    //Unit vector for the coordinate system
    constexpr Point up{0, 1, 0};
    constexpr Point left{-1, 0, 0};
    constexpr Point forward{0, 0, -1};

}
