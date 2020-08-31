//
// Created by kevin on 28/08/20.
//

#pragma once

#include <string>
#include <iostream>
#include "../Materials/ImageTexture.h"
#include "Loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Libs/stb_image.h"

class ImageTextureLoader : public Loader<ImageTexture> {
public:
    ImageTexture load(std::string filename) const {
        int width;
        int height;
        int channels;
        unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb);
        std::cout << filename << " Loaded with " << channels << " channels \n";

        Buffer<Color> data(width, height);

        int ptr = 0;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                float r = 0.0 + (static_cast<int>(img[ptr++]) - 0) * (1.0 - 0.0) / (255.0 - 0.0);
                float g = 0.0 + (static_cast<int>(img[ptr++]) - 0) * (1.0 - 0.0) / (255.0 - 0.0);
                float b = 0.0 + (static_cast<int>(img[ptr++]) - 0) * (1.0 - 0.0) / (255.0 - 0.0);
                data(i, j) = {r, g, b};
            }
        }

        stbi_image_free(img);

        return ImageTexture(std::move(data));
    }

};