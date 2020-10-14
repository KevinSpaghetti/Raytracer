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
#include "Textures/CheckerTexture.h"
#include "Mesh/PlaneMesh.h"
#include "Materials/Lambertian.h"
#include "Mesh/SphereMesh.h"
#include "Materials/Metal.h"
#include "Output/WindowOutput.h"
#include "Materials/Dielectric.h"
#include "Materials/SolidColorMaterial.h"
#include "Materials/SkyMaterial.h"
#include "Materials/PBRMaterial.h"

using namespace std::chrono_literals; //For second, milliseconds etc

EmptyNode createCornellBox(){
    EmptyNode root;

    auto checker_txt = std::make_shared<CheckerTexture>(Color{0, 0, 0}, Color{0.9, 0.9, 0.9});
    auto pot_geom = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    pot_geom->buildAccelerationStructure();

    auto check_mtl = std::make_shared<TXTMaterial>();
    check_mtl->addTexture("albedo", checker_txt);
    auto pot = std::make_shared<VisualNode>(pot_geom, std::make_shared<Lambertian>(Color(.65, .05, .05)));

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto grey = std::make_shared<Lambertian>(Color(.8, .8, .8));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto metal = std::make_shared<Metal>(Color(.12, .45, .15), 0.0);
    auto dielectric = std::make_shared<Dielectric>(Color{1.0, 1.0, 1.0}, 1.5);

    auto left = std::make_shared<PlaneMesh>(Point{-50, 0, 0},
                                            Normal{1, 0, 0});
    auto right = std::make_shared<PlaneMesh>(Point{50, 0, 0},
                                             Normal{-1, 0, 0});
    auto bottom = std::make_shared<PlaneMesh>(Point{0, -50, 0},
                                              Normal{0, 1, 0});
    auto top = std::make_shared<PlaneMesh>(Point{0, 50, 0},
                                           Normal{0, -1, 0});
    auto bg = std::make_shared<PlaneMesh>(Point{0, 0, -50},
                                          Normal{0, 0, 1});
    auto sp = std::make_shared<SphereMesh>(5);
    auto sp1 = std::make_shared<SphereMesh>(10);

    auto image_txt = std::make_shared<ImageTexture>(ImageTextureLoader().load("image.png"));
    auto image_mtl = std::make_shared<TXTMaterial>();
    image_mtl->addTexture("albedo", image_txt);

    auto spMetal = std::make_shared<VisualNode>(sp1, metal);
    auto spImage = std::make_shared<VisualNode>(sp, image_mtl);
    auto spDielectric = std::make_shared<VisualNode>(sp, dielectric);

    auto light = std::make_shared<AreaLightNode>(110.0f, 110.0f, Color{1.0, 1.0, 1.0}, 4.0f);
    light->translate({0, 50, 0});

    auto box = std::make_shared<EmptyNode>();
    auto spheres = std::make_shared<EmptyNode>();
    box->add(std::make_shared<VisualNode>(left, green));
    box->add(std::make_shared<VisualNode>(right, red));
    //box->add(std::make_shared<VisualNode>(top, white));
    box->add(std::make_shared<VisualNode>(bottom, white));
    box->add(std::make_shared<VisualNode>(bg, white));

    spImage->translate({0, -10, -20});
    spMetal->translate({-25, -30, -5});
    spDielectric->translate({15, -30, 10});

    spheres->add(spMetal);
    spheres->add(spDielectric);
    spheres->add(spImage);

    //spheres->rotate({0, 0, 1}, glm::radians(40.0f));
    spImage->translate({-5.0, 0, 30});
    spheres->translate({0.0, 0.0, 0.0});
    //root.add(cube);
    root.add(box);
    root.add(spheres);
    root.add(light);
    pot->rotate({0,1,0}, glm::radians(45.0f));
    pot->translate({0, -30, -20});
    root.add(pot);
    return root;
}

EmptyNode createTransformTest(){
    EmptyNode root;

    auto grey  = std::make_shared<Lambertian>(Color(.6,   .6,  .6));
    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto sg = std::make_shared<SphereMesh>(10.0f);

    auto pot_geom = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    pot_geom->buildAccelerationStructure();
    auto pot = std::make_shared<VisualNode>(pot_geom, std::make_shared<Lambertian>(Color(.65, .05, .05)));

    auto left = std::make_shared<VisualNode>(std::make_shared<SphereMesh>(3.5f), green);

    left->scale({2, 4, 2});
    left->rotate({0, 0, 1}, glm::radians(90.0f));
    left->translate({20, 10, 0});
    root.add(left);
    return root;
}

int main(){
    std::cout << "Creating buffer \n";
    const int width = 500;
    const int height = 500;
    Buffer<Color> color(width, height);
    glm::vec3 lookfrom(0,0,60); //TODO: remove look from, it is now the position of the node
    glm::vec3 lookat(0,0,0);
    auto dist_to_focus = glm::length(lookfrom - lookat);
    auto aperture = 0.1f;
    float aspect_ratio = width / height;

    auto scene = std::make_shared<EmptyNode>();
    scene->add(std::make_shared<EmptyNode>(createCornellBox()));
    std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>(lookfrom,
                                                                            lookat,
                                                                            consts::up,
                                                                            20,
                                                                            aspect_ratio,
                                                                            aperture,
                                                                            dist_to_focus);
    //We will pass the pointer to the node to the renderer to signal that this is
    //the active camera
    scene->add(camera);

    clock_t start = clock();

    std::cout << "Creating Renderer\n";

    Renderer renderer;
    renderer.pixelsamples() = 50;
    renderer.maxraydepth() = 12;
    renderer.max_depth_material() = std::make_shared<SolidColorMaterial>(Color{0.0, 0.0, 0.0});
    renderer.no_hit_material() = std::make_shared<SkyMaterial>();
    //Updater gets called every n milliseconds
    renderer.set_updater(1000ms,[](const Renderer::RenderInfo& info) -> void {
        if(info.stage != Renderer::Ended){
            std::cout << "\rCompleted " << info.samples_completed << "/" << info.samples_needed << "" << std::flush;
        }else{
            std::cout << "\n";
        }
    });

    std::cout << "Done \n";
    std::cout << "Rendering scene\n";
    renderer.render(scene, color, camera.get());
    std::cout << "Done \n";

    clock_t stop = clock();
    double seconds_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    int minutes_elapsed = std::floor(seconds_elapsed) / 60;
    printf("Render done in: %d minutes and %.5f seconds\n", minutes_elapsed,  seconds_elapsed - minutes_elapsed * 60);

    auto wo = WindowOutput(width,height, "Render", color);

    std::cout << "Writing Buffer \n";
    //Format the buffer for a ppm file format output
    auto formatted_buffer = std::make_unique<ColorBufferFormatPPM>(color);
    //Create a render output on a file
    auto output = std::make_unique<FileRenderOutput>("render.ppm");
    //Write the formatted output
    output->write(*formatted_buffer);
    std::cout << "Done \n" << std::endl;

    return 0;
}