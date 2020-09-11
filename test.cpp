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
#include "Mesh/TriangleMesh.h"
#include "Mesh/SphereMesh.h"
#include "Materials/CheckerTexture.h"
#include "Materials/SolidColorTexture.h"

Node createScene(){
    Node root;

    //Create teapot
    std::cout << "Loading teapot.obj \n";
    TriangleMesh geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";
    std::cout << "Loading texture\n";
    //ImageTexture image = ImageTextureLoader().load("./image.png");
    std::cout << "Done\n";
    TXTMaterial mat;
    mat.addTexture("albedo", std::make_shared<CheckerTexture>(CheckerTexture()));
    Node pot(std::make_shared<TriangleMesh>(geometry),
            std::make_shared<TXTMaterial>(mat));

    //Create Plane
    std::vector<Vertex> vt = {
            Vertex{-10, -4.0,-10},
            Vertex{-10, -4.0, 10},
            Vertex{ 10, -4.0, 10},
            Vertex{10, -4.0,-10}};
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

    pot.scale({0.3, 0.3, 0.3});

    root.add(make_shared<Node>(pot));

    return root;
}

int main(){

    std::cout << "Creating buffer \n";
    const int width = 500;
    const int height = 500;
    Buffer<Color> color(width, height);
    std::cout << "Done \n";

    Node scene = createScene();

    //Render
    Renderer::Configuration configuration{
        .pixel_samples = 16,
        .max_ray_depth = 64,
        //TODO: Add backface culling options
        .backface_culling = true
    };

    glm::vec3 lookfrom(0,4,6);
    glm::vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;
    float aspect_ratio = width / height;

    Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    clock_t start = clock();

    std::cout << "Creating Renderer\n";
    Renderer renderer(configuration);
    std::cout << "Done \n";
    std::cout << "Rendering scene\n";
    renderer.render(scene, color, cam);
    std::cout << "Done \n";

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("Render done in: %.5f seconds\n", elapsed);



    std::cout << "Writing Buffer \n";
    std::shared_ptr<ColorBufferFormat> formatted_buffer;
    std::shared_ptr<RenderOutput> output;

    //Format the buffer for a ppm file format output
    formatted_buffer = std::make_shared<ColorBufferFormatPPM>(color);
    //Create a render output on a file
    output = std::make_shared<FileRenderOutput>("render.ppm");
    //Write the formatted output
    output->write(*formatted_buffer);
    std::cout << "Done \n" << std::endl;

    return 0;
}