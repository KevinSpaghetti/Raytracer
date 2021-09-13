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
#include "SceneGraph/EmptyNode.h"
#include "SceneGraph/CameraNode.h"
#include "SceneGraph/VisualNode.h"
#include "SceneGraph/AreaLightNode.h"

using namespace std::chrono_literals; //For second, milliseconds etc

std::shared_ptr<EmptyNode> createBox(){

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto grey = std::make_shared<Lambertian>(Color(.8, .8, .8));
    auto blue = std::make_shared<Lambertian>(Color(.1, .2, .8));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto pbr = std::make_shared<PBRMaterial>(Color{1.0, 1.0, 1.0}, 0.0001f, 1.0f);

    auto image_txt = std::make_shared<ImageTexture>(ImageTextureLoader().load("image.png"));
    auto image_mtl = std::make_shared<TXTMaterial>();
    image_mtl->addTexture("albedo", image_txt);

    auto plane_mesh = std::make_shared<AARectMesh>(AARectMesh::Axis::XZ, Point{0, 0, 0}, Normal{0, 1, 0}, 100.0f, 100.0f);
    auto left{std::make_shared<VisualNode>(plane_mesh, green)};
    auto right{std::make_shared<VisualNode>(plane_mesh, red)};
    auto top{std::make_shared<VisualNode>(plane_mesh, white)};
    auto bottom{std::make_shared<VisualNode>(plane_mesh, white)};
    auto bg{std::make_shared<VisualNode>(plane_mesh, image_mtl)};
    auto front{std::make_shared<VisualNode>(plane_mesh, white)};
    left->translate({-50, 0, 0});
    right->translate({50, 0, 0});
    bottom->translate({0, -50, 0});
    top->translate({0, 50, 0});
    bg->translate({0, 0, -50});
    front->translate({0, 0, 50});
    left->rotate({0, 0, 1}, glm::radians(90.0f));
    right->rotate({0, 0, 1}, glm::radians(-90.0f));
    top->rotate({0, 0, 1}, glm::radians(-180.0f));
    bg->rotate({1, 0, 0}, glm::radians(-90.0f));
    front->rotate({1, 0, 0}, glm::radians(90.0f));
    auto box = std::make_shared<EmptyNode>();
    box->add(left);
    box->add(right);
    box->add(top);
    box->add(bottom);
    box->add(bg);
    box->add(front);

    return box;
}

EmptyNode createCornellBox(float light_dimension, float rough){
    EmptyNode root;

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto grey = std::make_shared<Lambertian>(Color(.8, .8, .8));
    auto blue = std::make_shared<Lambertian>(Color(.1, .2, .8));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto metal = std::make_shared<Metal>(Color(1.0, 1.0, 1.0), 0.0);
    auto dielectric = std::make_shared<Dielectric>(Color{1.0, 1.0, 1.0}, 1.5);
    auto pbr = std::make_shared<PBRMaterial>(Color{1.0, 1.0, 1.0}, rough, 1.0f);

    auto sp = std::make_shared<SphereMesh>(5);
    auto sp1 = std::make_shared<SphereMesh>(10);
    auto sp2 = std::make_shared<SphereMesh>(25);
    auto sp3 = std::make_shared<SphereMesh>(15);

    auto checker_txt = std::make_shared<CheckerTexture>(Color{0, 0, 0}, Color{0.9, 0.9, 0.9});
    auto pot_geom = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    auto suzanne_geom = std::make_shared<TriangleMesh>(OBJLoader().load("suzanne.obj"));
    pot_geom->buildAccelerationStructure();


    auto pot = std::make_shared<VisualNode>(sp1, pbr);
    auto suzanne = std::make_shared<VisualNode>(suzanne_geom, pbr);

    auto spMetal = std::make_shared<VisualNode>(sp1, metal);
    auto spDielectric = std::make_shared<VisualNode>(sp3, dielectric);
    auto spDiffuse = std::make_shared<VisualNode>(sp3, blue);

    auto light = std::make_shared<AreaLightNode>(light_dimension, light_dimension, Color{1.0, 1.0, 1.0}, 110.0f);
    light->translate({0, 49.988, 0});

    spMetal->translate({-25, -30, -5});
    //spDielectric->translate({15, 20, 10});

    auto spheres = std::make_shared<EmptyNode>();
    spheres->add(spMetal);
    //spheres->add(spDiffuse);
    //spheres->add(spDielectric);

    auto box = createBox();

    spMetal->translate({-12, -25, 0});
    //spDiffuse->translate({20, 0, 0});

    spDielectric->translate({0, 0, 0});
    spheres->translate({0.0, 0.0, 0.0});
    suzanne->scale({13, 13, 13});
    suzanne->translate({0, -20, 0});

    //pot->scale({1.2, 1.2, 1.2});
    //pot->rotate({0,1,0}, glm::radians(-25.0f));
    pot->translate({12, -25, 0});

    root.add(light);
    root.add(box);
    root.add(spheres);
    root.add(pot);
    //root.add(suzanne);
    return root;
}

EmptyNode createSpheres(){
    EmptyNode root;

    auto grey  = std::make_shared<Lambertian>(Color(.6,   .6,  .6));
    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Metal>(Color(.12, .45, .15), 0.01f);
    auto metal = std::make_shared<Metal>(Color(.12, .45, .15), 0.01f);
    auto dielectric = std::make_shared<Dielectric>(Color{1.0, 1.0, 1.0}, 1.5);

    auto pbr = std::make_shared<PBRMaterial>(
            Color{0.67, 0.36, 0.49},
            1.0, 1.2);

    //auto pot_geom = std::make_shared<TriangleMesh>(OBJLoader().load("teapot.obj"));
    //pot_geom->buildAccelerationStructure();
    //auto model = std::make_shared<VisualNode>(pot_geom, pbr);

    auto smallg = std::make_shared<SphereMesh>(0.5f);
    auto smallghollow = std::make_shared<SphereMesh>(-0.5f);
    auto s1 = std::make_shared<VisualNode>(smallg, metal);
    auto s2 = std::make_shared<VisualNode>(smallg, red);
    auto s3 = std::make_shared<VisualNode>(smallg, dielectric);

    auto bigg = std::make_shared<SphereMesh>(100.0f);
    auto big = std::make_shared<VisualNode>(bigg, grey);
    big->translate({0,-100.5 ,0});

    EmptyNode n;

    s3->translate({0, 0, 0});
    n.add(s3); //dielectric

    //root.add(model);
    n.add(big);
    n.translate({0, 0, -1});
    root.add(std::make_shared<EmptyNode>(n));
    return root;
}


int main(){
    const int width = 500;
    const int height = 500;

    //int samples = 5;
    int depth = 5;

    std::vector<int> sample_n{1000, 10, 10};//{10, 64, 52, 1024};
    std::vector<float> lights_dim{0.001f, 0.25f, 0.5f, 0.75f ,1.0f};

    for (int i = 0; i < 1; ++i) {
        int samples = sample_n[i];
        float light_dim = lights_dim[i];

        std::cout << "Creating buffer \n";
        Buffer<Color> color(width, height);
        glm::vec3 lookfrom(0, 0, 49);
        glm::vec3 lookat(0, -30, 0);
        auto focus_distance = glm::length(lookat - lookfrom);
        auto aperture = 0.5f;
        float aspect_ratio = width / height;
        auto scene = std::make_shared<EmptyNode>();
        scene->add(std::make_shared<EmptyNode>(createSpheres()));
        std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>(lookfrom, lookat, consts::up, 20,
                                                                          aspect_ratio, aperture, focus_distance);
        //We will pass the pointer to the node to the renderer to signal that this is
        //the active camera
        scene->add(camera);
        clock_t start = clock();
        std::cout << "Creating Renderer\n";
        Renderer renderer;
        renderer.pixelsamples() = samples;
        renderer.maxraydepth() = depth;
        renderer.max_depth_material() = std::make_shared<SolidColorMaterial>(Color{0.0, 0.0, 0.0});
        renderer.no_hit_material() = std::make_shared<SolidColorMaterial>(Color{0.4, 0.4, 0.4});
        //Updater gets called every n milliseconds
        renderer.set_updater(1000ms, [](const Renderer::RenderInfo &info) -> void {
            if (info.stage != Renderer::RenderStage::Ended) {
                std::cout << "\rCompleted " << info.samples_completed << "/" << info.samples_needed << "" << std::flush;
            } else {
                std::cout << "\n";
            }
        });

        std::cout << "Done \n";
        std::cout << "Rendering scene\n";
        renderer.render(scene, color, camera.get());
        std::cout << "Done \n";

        clock_t stop = clock();
        double seconds_elapsed = static_cast<double>(stop - start) / CLOCKS_PER_SEC;
        int minutes_elapsed = static_cast<int>(std::floor(seconds_elapsed)) / 60;
        printf("Render done in: %d minutes and %.5f seconds\n", minutes_elapsed,
               seconds_elapsed - minutes_elapsed * 60);

        //auto wo = WindowOutput(width, height, "Render", color);

        std::cout << "Writing Buffer \n";
        //Format the buffer for a ppm file format output
        auto formatted_buffer = std::make_unique<ColorBufferFormatPPM>(color);
        //Create a render output on a file
        std::string filename = "render" + std::to_string(samples) + "s" + std::to_string(depth) + "MF" + std::to_string(light_dim) + "MFModel" + ".ppm";
        auto output = std::make_unique<FileRenderOutput>(filename);
        //Write the formatted output
        output->write(*formatted_buffer);
        std::cout << "Done \n" << std::endl;
    }

    return 0;
}