//
// Created by kevin on 17/08/20.
//

#include "SceneGraph/Node.h"
#include "Loader/OBJLoader.h"
#include "Utils/Buffer.h"
#include "Output/ColorBufferFormatPPM.h"
#include "Output/FileRenderOutput.h"
#include "Renderer.h"
#include "Loader/ImageTextureLoader.h"
#include "Materials/TXTMaterial.h"
#include "Mesh/TriangleMesh.h"
#include "Textures/SolidColorTexture.h"
#include "Textures/CheckerTexture.h"
#include "Mesh/PlaneMesh.h"
#include "Materials/Lambertian.h"
#include "Mesh/SphereMesh.h"
#include "Materials/Metal.h"
#include "Materials/Dielectric.h"

Node createScene(){
    Node root;

    //Create teapot
    std::cout << "Loading teapot.obj \n";
    std::shared_ptr<TriangleMesh> geometry = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    geometry->buildAccelerationStructure();
    std::cout << "Done \n";
    std::cout << "Loading Texture \n";
    ImageTexture txt = ImageTextureLoader().load("image.png");
    std::cout << "Done";

    auto material_lambert = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    auto material_metal = make_shared<Metal>(Color(0.8, 0.8, 0.8));
    auto material_dielectric = make_shared<Dielectric>(0.01);

    Node terrain(make_shared<SphereMesh>(Point{0.0, -100.5, -1.0}, 100.0), material_lambert);
    Node sp2(make_shared<SphereMesh>(Point{0.0,    0.0, -1.0},   0.5), material_metal);
    Node sp3(make_shared<SphereMesh>(Point{0.0,    0.0, -1.0},   0.5), material_lambert);

    /*
    sp2.translate({-0.5, 0.0, 0.0});
    sp3.translate({ 0.5, 1.0, 0.0});
    root.add(make_shared<Node>(sp2));
    //sp3.rotate({0.0, 1.0, 0.0}, glm::degrees(45.0f));
    root.add(make_shared<Node>(sp3));
    */

    root.add(make_shared<Node>(terrain));


    Node pot(geometry, material_metal);
    pot.translate({0.0, 1.0 , 0.0});
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
        .pixel_samples = 2,
        .max_ray_depth = 4,
        //TODO: Add backface culling options
        .backface_culling = true
    };

    glm::vec3 lookfrom(0,4.0,4);
    glm::vec3 lookat(0,0,-1);
    auto dist_to_focus = glm::length(lookfrom - lookat);
    auto aperture = 0.1;
    float aspect_ratio = width / height;

    Camera camera(lookfrom, lookat, consts::up, 20, aspect_ratio, aperture, dist_to_focus);

    clock_t start = clock();

    std::cout << "Creating Renderer\n";
    Renderer renderer(configuration);
    std::cout << "Done \n";
    std::cout << "Rendering scene\n";
    renderer.render(scene, color, camera);
    std::cout << "Done \n";

    clock_t stop = clock();
    double seconds_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    int minutes_elapsed = std::floor(seconds_elapsed) / 60;
    printf("Render done in: %d minutes and %.5f seconds\n", minutes_elapsed,  seconds_elapsed - minutes_elapsed * 60);


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