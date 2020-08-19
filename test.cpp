//
// Created by kevin on 17/08/20.
//

#include "SceneGraph/Node.h"
#include "Loader/OBJLoader.h"
#include "Utils/Buffer.h"
#include "Utils/ColorBufferFormatPPM.h"
#include "Utils/FileRenderOutput.h"

int main(){

    std::cout << "Loading teapot.obj \n";
    Geometry geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";

    std::cout << "Creating buffer \n";
    Buffer<Color> color(1920, 1080);
    for (int i = 0; i < color.getHeight(); ++i) {
        for (int j = 0; j < color.getWidth(); ++j) {
            color(i, j) = {1, 1, 1};
        }
    }
    std::cout << "Done \n";

    std::cout << "Writing Buffer \n";
    std::shared_ptr<ColorBufferFormat> formatted_buffer;
    std::shared_ptr<RenderOutput> output;

    formatted_buffer = std::make_shared<ColorBufferFormatPPM>(color);
    output = std::make_shared<FileRenderOutput>("render.ppm");
    output->write(*formatted_buffer);
    std::cout << "Done \n" << std::endl;

    return 0;
}