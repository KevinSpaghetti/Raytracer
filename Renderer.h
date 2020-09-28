//
// Created by kevin on 20/08/20.
//

#pragma once

#include <optional>
#include <thread>
#include <atomic>
#include "SceneGraph/Node.h"
#include "Utils/Buffer.h"
#include "Utils/Tile.h"
#include "Utils/Camera.h"
#include "Geom/Intersection.h"
#include "Utils/Consts.h"
#include "Materials/VNMaterial.h"
#include "Utils/Random.h"
#include "BVH/BVH.h"
#include "Geom/ObjectIntersection.h"
#include "Integrators/BackwardIntegrator.h"
#include "Integrators/NormalDebugIntegrator.h"

template<typename Integrator = BackwardIntegrator>
class Renderer {
private:
    //Renderer configuration
    struct Configuration {
        int pixel_samples = 1;
        int max_ray_depth = 1;
        bool backface_culling = false;
    } configuration;
public:

    Renderer() : configuration() {}

    //For now render only color
    void render(Node graph, Buffer<Color>& target, Camera cam){
        stage = Started;

        //Divide the buffer in tiles
        //int tile_number = 1;
        std::vector<Tile<Color>> tiles = Tile<Color>::split(target, tile_number);
        int n_tile = 1;
        for (Tile<Color> t : tiles) {
            glm::vec2 tile_dimensions = t.getDimensions();
            glm::vec2 tile_start = t.getStart();
            std::cout << n_tile << " tile\n"
                      << "\t Position ( " << tile_start.x << " , " << tile_start.y << ")\n"
                      << "\t Size     ( " << tile_dimensions.x  << " , " << tile_dimensions.y << ")\n";
            ++n_tile;
        }

        //Save the camera
        camera = cam;

        //Save the scene and build the bvh
        scene = graph;
        std::cout << "Building the bvh\n";
        bvh = BVH(graph);
        std::cout << "Done\n";

        //Calculate samples needed to finish
        for (Tile<Color> tile : tiles) {
            samples_needed += tile.getDimensions().x * tile.getDimensions().y * configuration.pixel_samples;
        }

        std::cout << "Need " << samples_needed << " Samples\n";
        //Raytrace the scene
        std::vector<std::thread> workers;

        //Create the sampler
        sampler = std::make_unique<Integrator>(&bvh, BackwardIntegrator::SceneSamplerConfiguration{
                configuration.max_ray_depth,
                configuration.backface_culling
        });

        //Thread that updates the stats about the samples completed
        workers.emplace_back(std::thread(&Renderer::update_stats, this));
        for (Tile<Color> tile : tiles){
            workers.emplace_back(std::thread(&Renderer::trace_tile, this, tile));
        }

        for (std::thread& worker : workers){
            worker.join();
        }

        stage = Ended;

    }

    int& pixelsamples(){
        return configuration.pixel_samples;
    }

    int& maxraydepth(){
        return configuration.max_ray_depth;
    }

    bool& backfaceculling(){
        return configuration.backface_culling;
    }

private:

    //TODO: Extract logic in object
    void update_stats(){
        //We need to sleep the thread for seconds, or else the fast reads on samples_completed
        //block the other threads (the workers) from executing and we have a slow down
        while(samples_completed < samples_needed){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "\rCompleted " << samples_completed << "/" << samples_needed << "" << std::flush;
        }
        std::cout << "\n";
    }

    void trace_tile(Tile<Color> tile){
        glm::vec2 tile_dimensions = tile.getDimensions();
        for (int i = 0; i < tile_dimensions.y; ++i) {
            for (int j = 0; j < tile_dimensions.x; ++j) {
                Color pixel_color{0, 0, 0};
                for (int sample = 0; sample < configuration.pixel_samples; ++sample) {
                    glm::vec2 random_2D = {randomized::scalar::random(-1, 1), randomized::scalar::random(-1, 1)};
                    glm::vec2 sample_coords = (tile.getStart() + glm::vec2{j, i} + random_2D) / (tile.getDimensions() * static_cast<float>(tile_number));

                    Ray r = camera.get(sample_coords.x, sample_coords.y);

                    pixel_color += sampler->sample(r);
                }
                tile(i, j) = pixel_color / static_cast<float>(configuration.pixel_samples);
            }
            //Update samples every line to avoid lock overhead
            samples_completed += tile_dimensions.x * configuration.pixel_samples;
        }
    }


private:
    enum RenderStage {
        Started,
        Ended
    } stage;
    std::unique_ptr<BackwardIntegrator> sampler;
    int tile_number = 1;
    Camera camera;
    Node scene;
    BVH bvh;

    std::atomic<int> samples_completed = 0;
    int samples_needed = 0;
};
