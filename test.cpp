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

Node createScene(){
    Node root;

    //Create teapot
    std::cout << "Loading teapot.obj \n";
    TriangleMesh geometry = OBJLoader().load("teapot.obj");
    std::cout << "Done \n";
    std::cout << "Loading texture\n";
    ImageTexture image = ImageTextureLoader().load("./image.png");
    std::cout << "Done\n";

    auto sct = [](Ray r, Intersection i){
        return Color{0, 0, 0};
    };
    auto clr = [](Ray r, Intersection i){
        return Ray(i.pv, i.pn);
    };
    HitShader checkerShader(sct, clr);

    Node pot(std::make_shared<Mesh>(geometry),
            std::make_shared<HitShader>(checkerShader));

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

    root.add(pot);

    return root;
}

int main(){

    std::cout << "Creating buffer \n";
    const int width = 1920;
    const int height = 1080;
    Buffer<Color> color(width, height);
    std::cout << "Done \n";

    Node scene = createScene();

    NoHitShader no_hit([](Ray r){
        glm::vec3 dir = glm::normalize(r.getDirection());
        float t = 0.5f * (dir.y + 1.0);
        return (1.0f - t) * Color{0.8, 0.8, 0.8} + t * Color{0.2, 0.4, 0.7};
    });
    MaxDepthShader max_depth([](Ray r){
        return Color{0, 0, 0};
    });

    //Render
    Renderer::Configuration configuration{
        .pixel_samples = 128,
        .max_ray_depth = 64,
        //TODO: Add backface culling options
        .backface_culling = true,
        .max_depth_shader = max_depth,
        .no_hit_shader = no_hit
    };

    configuration.no_hit_shader = no_hit;
    configuration.max_depth_shader = max_depth;

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