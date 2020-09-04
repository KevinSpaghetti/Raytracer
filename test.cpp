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
#include "SceneGraph/TriangleMesh.h"
#include "SceneGraph/SphereMesh.h"

Node createScene(){
    Node root;

    //Create teapot
    std::cout << "Loading teapot.obj \n";
    TriangleMesh geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";
    std::cout << "Loading texture\n";
    ImageTexture image = ImageTextureLoader().load("./image.png");
    TXTMaterial material;
    material.addTexture("albedo", std::make_shared<ImageTexture>(image));
    std::cout << "Done\n";
    std::shared_ptr<Mesh> geom = std::make_shared<TriangleMesh>(geometry);
    std::shared_ptr<Material> mat = std::make_shared<TXTMaterial>(material);
    Node pot(geom, mat);

    //Create Plane
    std::vector<Vertex> vt = {
            Vertex{-5, -2.0,-5},
            Vertex{-5, -2.0, 10},
            Vertex{ 5, -2.0, 10},
            Vertex{5, -2.0,-5}};
    std::vector<Triangle> tr = {
            Triangle{1, 2, 3},
            Triangle{1, 3, 4}};
    std::vector<Normal> nm = {
            Normal{0.0, 1.0, 0.0},
            Normal{0.0, 1.0, 0.0},
            Normal{0.0, 1.0, 0.0},
            Normal{0.0, 1.0, 0.0}};
    std::vector<UV> uv = {
            UV{0.0, 1.0, 0.0},
            UV{0.0, 0.0, 0.0},
            UV{1.0, 0.0, 0.0},
            UV{1.0, 1.0, 0.0}};
    std::shared_ptr<Mesh> triangle = std::make_shared<TriangleMesh>(vt,tr,nm,uv);
    Node plane(triangle, mat);

    std::shared_ptr<Mesh> sphere_geom = std::make_shared<SphereMesh>(glm::vec3{0.0, 0.0, -1.0}, 3);
    Node sphere(sphere_geom, mat);

    std::shared_ptr<Mesh> terrain_geom = std::make_shared<SphereMesh>(glm::vec3{0.0, -100.5, -1.0}, 100);
    Node terrain(terrain_geom, mat);

    //root.add(plane);
    root.add(sphere);
    root.add(terrain);

    return root;
}

int main(){

    std::cout << "Creating buffer \n";
    Buffer<Color> color(1000, 1000);
    std::cout << "Done \n";

    Node scene = createScene();

    //Render
    Renderer::Configuration configuration;
    configuration.pixel_samples = 4;
    configuration.max_ray_depth = 4;
    configuration.backface_culling = true;

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