//
// Created by kevin on 17/08/20.
//

#include "SceneGraph/Node.h"
#include "Loader/OBJLoader.h"
#include "Utils/Buffer.h"
#include "Utils/ColorBufferFormatPPM.h"
#include "Utils/FileRenderOutput.h"
#include "Renderer.h"

int main(){

    std::cout << "Loading teapot.obj \n";
    Geometry geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";

    std::cout << "Creating buffer \n";
    Buffer<Color> color(100, 100);
    std::cout << "Done \n";

    Node scene(make_shared<Geometry>(geometry), nullptr);

    //Render
    Renderer::Configuration configuration;
    configuration.pixel_samples = 4;
    configuration.max_ray_depth = 8;

    glm::vec3 lookfrom(26,3,6);
    glm::vec3 lookat(0,2,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;
    float aspect_ratio = 16.0f / 9.0f;
    Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    std::cout << "Creating Renderer\n";
    Renderer renderer(configuration);
    std::cout << "Renderer creation done\n";
    std::cout << "Rendering scene\n";
    renderer.render(scene, color, cam);
    std::cout << "Scene rendered\n";


    std::cout << "Writing Buffer \n";
    std::shared_ptr<ColorBufferFormat> formatted_buffer;
    std::shared_ptr<RenderOutput> output;

    formatted_buffer = std::make_shared<ColorBufferFormatPPM>(color);
    output = std::make_shared<FileRenderOutput>("render.ppm");
    output->write(*formatted_buffer);
    std::cout << "Done \n" << std::endl;

    return 0;
}