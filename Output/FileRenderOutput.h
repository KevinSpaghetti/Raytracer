//
// Created by kevin on 19/08/20.
//

#pragma once

#include <fstream>

#include "RenderOutput.h"
#include "../Utils/Structs.h"
#include "ColorBufferFormat.h"

class FileRenderOutput : public RenderOutput {
public:
    FileRenderOutput(std::string filename, std::string base_path = "./") : filename(filename), base_path(base_path) {};

    void write(ColorBufferFormat& format) const override {
        std::string fullpath = base_path + filename;

        std::ofstream file;
        file.open(fullpath.data());

        file << format.serialize();

        file.close();
    }


private:
    std::string base_path;
    std::string filename;

};
