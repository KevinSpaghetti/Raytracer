//
// Created by kevin on 28/08/20.
//

#pragma once

#include "../Utils/Structs.h"
#include "../Utils/Buffer.h"
#include "Texture.h"

class ImageTexture : public Texture {
public:

    ImageTexture(Buffer<Color> b) : data(b){}

    Color value(UV at) {
        //To get the height we need to invert the y axis of the buffer
        int height = static_cast<float>(data.getHeight()-1) * at.y;
        int width = static_cast<float>(data.getWidth()-1) * at.x;
        return data(height, width);
    }

    Buffer<Color>& buffer(){
        return data;
    }
private:
    Buffer<Color> data;

};