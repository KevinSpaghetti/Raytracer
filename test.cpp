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
            Vertex{-10, -1.0,-10},
            Vertex{-10, -1.0, 10},
            Vertex{ 10, -1.0, 10},
            Vertex{10, -1.0,-10}};
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
    std::shared_ptr<Material> nmat = std::make_shared<VNMaterial>();
    Node plane(triangle, nmat);

    std::shared_ptr<Mesh> sp1 = std::make_shared<SphereMesh>(glm::vec3{-4.0, 1.5, -1.0}, 1);
    std::shared_ptr<Mesh> sp2 = std::make_shared<SphereMesh>(glm::vec3{-1.5, 1.5, -1.0}, 1);
    std::shared_ptr<Mesh> sp3 = std::make_shared<SphereMesh>(glm::vec3{1.5, 1.5, -1.0}, 1);
    std::shared_ptr<Mesh> sp4 = std::make_shared<SphereMesh>(glm::vec3{4.0, 1.5, -1.0}, 1);

    Node sphere1(sp1, mat);
    Node sphere2(sp2, mat);
    Node sphere3(sp3, mat);
    Node sphere4(sp4, mat);

    root.add(plane);
    Node spheres;
    spheres.add(sphere1);
    spheres.add(sphere2);
    spheres.add(sphere3);
    spheres.add(sphere4);
    root.add(spheres);

    return root;
}

int main(){

    std::cout << "Creating buffer \n";
    Buffer<Color> color(1000, 1000);
    std::cout << "Done \n";

    Node scene = createScene();

    //Render
    Renderer::Configuration configuration;
    configuration.pixel_samples = 8;
    configuration.max_ray_depth = 10;
    //TODO: Add backface culling options
    configuration.backface_culling = true;

    glm::vec3 lookfrom(0,0,1);
    glm::vec3 lookat(0,0,-1);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;
    float aspect_ratio = 1.0f / 1.0f;
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

    formatted_buffer = std::make_shared<ColorBufferFormatPPM>(color);
    output = std::make_shared<FileRenderOutput>("render.ppm");
    output->write(*formatted_buffer);
    std::cout << "Done \n" << std::endl;

    return 0;
}