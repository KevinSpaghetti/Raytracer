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
#include "Materials/DiffuseLight.h"
#include "Output/WindowOutput.h"
#include "Materials/Dielectric.h"
#include "Materials/SolidColorMaterial.h"
#include "Materials/SkyMaterial.h"
#include "Mesh/AARectMesh.h"

using namespace std::chrono_literals; //For second, milliseconds etc

Node createCornellBox(){
    Node root;

    auto checker_txt = std::make_shared<CheckerTexture>(Color{0, 0, 0}, Color{0.8, 0.8, 0.8});
    auto pot_geom = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    pot_geom->buildAccelerationStructure();

    auto check_mtl = std::make_shared<TXTMaterial>();
    check_mtl->addTexture("albedo", checker_txt);
    auto pot = std::make_shared<Node>(pot_geom, check_mtl);

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto metal = std::make_shared<Metal>(Color(.12, .45, .15), 0.0);
    auto dielectric = std::make_shared<Dielectric>(1.5);

    auto light = std::make_shared<DiffuseLight>(Color(1, 1, 1), 2.0f);

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

    auto sp = std::make_shared<SphereMesh>(Point{0, 0, 0}, 20);
    auto sp1 = std::make_shared<SphereMesh>(Point{-30, -30, 20}, 10);
    auto sp2 = std::make_shared<SphereMesh>(Point{30, -30, 10}, 15);
    auto sp3 = std::make_shared<SphereMesh>(Point{25, -25, 30}, -10);

    auto light_plane = std::make_shared<AARectMesh>(AARectMesh::Axis::XZ, Point{0, 0, 0}, 70.0f, 70.0f);

    //auto nsphere = std::make_shared<Node>(sp, dielectric);
    auto nsphere1 = std::make_shared<Node>(sp1, metal);
    auto image_txt = std::make_shared<ImageTexture>(ImageTextureLoader().load("image.png"));
    auto image_mtl = std::make_shared<TXTMaterial>();
    image_mtl->addTexture("albedo", image_txt);
    auto nsphere2 = std::make_shared<Node>(sp2, image_mtl);
    auto nsphere3 = std::make_shared<Node>(sp3, dielectric);

    auto nlight = std::make_shared<Node>(light_plane, light);
    auto nleft = std::make_shared<Node>(left, green);
    auto nright = std::make_shared<Node>(right, red);
    auto ntop = std::make_shared<Node>(top, light);
    auto nbottom = std::make_shared<Node>(bottom, white);
    auto nbg = std::make_shared<Node>(bg, white);

    auto box = std::make_shared<Node>();
    auto spheres = std::make_shared<Node>();
    box->add(nleft);
    box->add(nright);
    box->add(ntop);
    box->add(nbottom);
    box->add(nbg);


    spheres->add(nsphere1);
    spheres->add(nsphere2);

    spheres->translate({0, 40, -10});
    nlight->translate({0, 49.999, 0});
    pot->scale({1.5, 1.5, 1.5});
    pot->translate({0, -40, -10});

    root.add(box);
    root.add(spheres);
    //root.add(nsphere3);
    //root.add(nlight);
    root.add(pot);
    return root;
}

Node createSpheres(){
    Node root;

    auto grey  = std::make_shared<Lambertian>(Color(.6,   .6,  .6));
    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto metal = std::make_shared<Metal>(Color(.12, .45, .15), 0.0);
    auto dielectric = std::make_shared<Dielectric>(1.5);

    auto smallg = std::make_shared<SphereMesh>(Point{0,0 ,-1}, 0.5f);
    auto small = std::make_shared<Node>(smallg, dielectric);
    auto bigg = std::make_shared<SphereMesh>(Point{0,-100.5 ,-1}, 100.0f);
    auto big = std::make_shared<Node>(bigg, grey);

    root.add(small);
    root.add(big);

    return root;
}

int main(){

    std::cout << "Creating buffer \n";
    const int width = 500;
    const int height = 500;
    Buffer<Color> color(width, height);

    Node scene = createCornellBox();

    glm::vec3 lookfrom(0,1,80);
    glm::vec3 lookat(0,0,0);
    auto dist_to_focus = glm::length(lookfrom - lookat);
    auto aperture = 0.1f;
    float aspect_ratio = width / height;

    Camera camera(lookfrom, lookat, consts::up, 20, aspect_ratio, aperture, dist_to_focus);

    clock_t start = clock();

    std::cout << "Creating Renderer\n";

    Renderer renderer;
    renderer.pixelsamples() = 10;
    renderer.maxraydepth() = 10;
    renderer.backfaceculling() = false;
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
    renderer.render(scene, color, camera);
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