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


Node createCornellBox(){
    Node root;

    auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(1, 1, 1), 2.0f);

    auto left = std::make_shared<PlaneMesh>(Point{-5, 0, 0},
                                            Normal{1, 0, 0});
    auto right = std::make_shared<PlaneMesh>(Point{5, 0, 0},
                                             Normal{-1, 0, 0});
    auto bottom = std::make_shared<PlaneMesh>(Point{0, -5, 0},
                                              Normal{0, 1, 0});
    auto top = std::make_shared<PlaneMesh>(Point{0, 5, 0},
                                           Normal{0, -1, 0});
    auto bg = std::make_shared<PlaneMesh>(Point{0, 0, -5},
                                          Normal{0, 0, 1});

    auto sp = std::make_shared<SphereMesh>(Point{0, 0, 0}, 2);

    auto nsphere = make_shared<Node>(sp, red);
    auto nleft = make_shared<Node>(left, green);
    auto nright = make_shared<Node>(right, red);
    auto ntop = make_shared<Node>(top, light);
    auto nbottom = make_shared<Node>(bottom, white);
    auto nbg = make_shared<Node>(bg, white);

    //TODO: Don't use normal for direction typedef a Direction type

    auto box = std::make_shared<Node>();
    box->add(nleft);
    box->add(nright);
    box->add(ntop);
    box->add(nbottom);
    box->add(nbg);

    root.add(box);
    root.add(nsphere);
    return root;
}

Node createSpheres(){
    Node root;

    auto grey  = make_shared<Lambertian>(Color(.6,   .6,  .6));
    auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));

    auto smallg = std::make_shared<SphereMesh>(Point{0,0 ,-1}, 0.5f);
    auto small = std::make_shared<Node>(smallg, grey);
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
    std::cout << "Done \n";

    Node scene = createSpheres();

    //Render
    Renderer::Configuration configuration{
            .pixel_samples = 50,
            .max_ray_depth = 10,
            .backface_culling = false
    };

    glm::vec3 lookfrom(0,0,1);
    glm::vec3 lookat(0,0,0);
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