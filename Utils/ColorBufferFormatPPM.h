//
// Created by kevin on 19/08/20.
//

#pragma once

#include <sstream>

#include "Buffer.h"
#include "Structs.h"
#include "ColorBufferFormat.h"

class ColorBufferFormatPPM : public ColorBufferFormat {
public:
    ColorBufferFormatPPM(Buffer<Color> buffer) : buffer(buffer){};

    std::string serialize() {
        std::ostringstream result;
        result << "P3" << "\n";
        result << buffer.getWidth() << ' ' << buffer.getHeight() << '\n' << 255 << '\n';

        for (int i = 0; i < buffer.getHeight(); ++i) {
            for (int j = 0; j < buffer.getWidth(); ++j) {
                Color c = buffer(i, j);
                result << static_cast<int>(c.r * 255) << " "
                       << static_cast<int>(c.g * 255) << " "
                       << static_cast<int>(c.b * 255) << "\n";
            }
        }
        return result.str();
    };

private:
    Buffer<Color> buffer;
};
