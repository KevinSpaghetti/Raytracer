//
// Created by kevin on 19/08/20.
//

#pragma once

#include <sstream>

#include "../Utils/Buffer.h"
#include "../Utils/Structs.h"
#include "ColorBufferFormat.h"

class ColorBufferFormatPPM : public ColorBufferFormat {
public:
    ColorBufferFormatPPM(const Buffer<Color>& buffer) : buffer(buffer){};

    std::string serialize() const override {
        std::ostringstream result;
        result << "P3" << "\n";
        result << buffer.getWidth() << ' ' << buffer.getHeight() << '\n' << 255 << '\n';

        for (int i = 0; i < buffer.getHeight(); ++i) {
            for (int j = 0; j < buffer.getWidth(); ++j) {
                Color c = buffer(i, j);
                Color g = glm::clamp(glm::sqrt(c) * 255.0f, Color(0), Color(255));
                result << static_cast<int>(g.r) << " "
                       << static_cast<int>(g.g) << " "
                       << static_cast<int>(g.b) << "\n";
            }
        }
        return result.str();
    };

private:
    const Buffer<Color>& buffer;
};
