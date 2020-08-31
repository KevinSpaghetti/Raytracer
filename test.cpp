//
// Created by kevin on 17/08/20.
//

#include "SceneGraph/Node.h"
#include "Loader/OBJLoader.h"
#include "Utils/Buffer.h"
#include "Utils/ColorBufferFormatPPM.h"
#include "Utils/FileRenderOutput.h"
#include "Renderer.h"
#include "Materials/VNMaterial.h"
#include "Materials/ImageTexture.h"
#include "Loader/ImageTextureLoader.h"
#include "Materials/TXTMaterial.h"

int main(){

    std::cout << "Loading teapot.obj \n";
    Mesh geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";

    std::vector<Vertex> vt = {
            Vertex{0.0, 1.0, 1.0},
            Vertex{-1.0, 0.0, 1.0},
            Vertex{1.0, 0.0, 1.0},
            Vertex{0.0, 1.0, 1.0}};
    std::vector<Triangle> tr = {
            Triangle{1, 2, 3}};
    std::vector<Normal> nm = {
            Normal{1.0, 0.0, 0.0},
            Normal{0.0, 1.0, 0.0},
            Normal{0.0, 0.0, 1.0},
            Normal{0.0, 1.0, 1.0}};
    std::vector<UV> uv = {
            UV{0.0, 0.0, 0.0},
            UV{1.0, 0.0, 0.0},
            UV{0.5, 1.0, 0.0},
            UV{1.0, 1.0, 0.0}};
    Mesh triangle = Mesh(vt, tr, nm, uv);

    std::cout << "Loading texture\n";
    ImageTexture image = ImageTextureLoader().load("./image.png");
    TXTMaterial material;
    material.addTexture("albedo", std::make_shared<ImageTexture>(image));
    std::cout << "Done\n";

    std::cout << "Creating buffer \n";
    Buffer<Color> color(1000, 1000);
    std::cout << "Done \n";

    std::shared_ptr<Mesh> geom = std::make_shared<Mesh>(geometry);
    std::shared_ptr<Material> mat = std::make_shared<TXTMaterial>(material);

    Node scene(geom, mat);
    scene.translate(glm::vec3{0,-1.0,1.0});

    /*
    Node n1(geom, mat);
    Node n2(geom, mat);
    n1.translate(glm::vec3{1.0, 1.0, 0});
    scene.add(n1);
    n2.translate(glm::vec3{-1.0, -1.0, 0});
    scene.add(n2);
    */


    //Render
    Renderer::Configuration configuration;
    configuration.pixel_samples = 4;
    configuration.max_ray_depth = 8;

    glm::vec3 lookfrom(0,0,1);
    glm::vec3 lookat(0,0,-1);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;
    float aspect_ratio = 1.0f / 1.0f;
    Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    std::cout << "Creating Renderer\n";
    Renderer renderer(configuration);
    std::cout << "Done \n";
    std::cout << "Rendering scene\n";
    renderer.render(scene, color, cam);
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