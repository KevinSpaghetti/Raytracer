//
// Created by kevin on 28/08/20.
//

#pragma once

#include <string>
#include <iostream>
#include "../Textures/ImageTexture.h"
#include "Loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Libs/stb_image.h"

class ImageTextureLoader : public Loader<ImageTexture> {
public:
    ImageTexture load(std::string filename) const {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb);
        std::cout << filename << " Loaded with " << channels << " channels \n";

        Buffer<Color> data(width, height);

        //Remap the rgb from 0-255 to 0-1
        int ptr = 0;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                float r = static_cast<int>(img[ptr++]) * (1.0f - 0.0f) / (255.0f - 0.0f);
                float g = static_cast<int>(img[ptr++]) * (1.0f - 0.0f) / (255.0f - 0.0f);
                float b = static_cast<int>(img[ptr++]) * (1.0f - 0.0f) / (255.0f - 0.0f);
                data(i, j) = {r, g, b};
            }
        }

        stbi_image_free(img);

        return ImageTexture(std::move(data));
    }

};