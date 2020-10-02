//
// Created by kevin on 20/08/20.
//

#pragma once

#include <optional>
#include <thread>
#include <atomic>
#include <functional>
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

class Renderer {
private:
    //Renderer configuration
    struct Configuration {
        int pixel_samples = 1;
        int max_ray_depth = 1;
        std::shared_ptr<Material> max_ray_depth_material;
        std::shared_ptr<Material> no_hit_material;
    } configuration;

public:
    enum RenderStage {
        Started,
        Ended
    };
    struct RenderInfo {
        std::atomic<int> samples_completed = 0;
        int samples_needed = 0;
        RenderStage stage;
    } render_info;
public:

    Renderer() : configuration() {}

    //For now render only color
    void render(Node graph, Buffer<Color>& target, Camera cam){
        render_info.stage = Started;
        //Save the camera
        camera = cam;
        //Save the scene and build the bvh
        scene = graph;

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


        std::cout << "Building the bvh\n";
        bvh = BVH(graph);
        std::cout << "Done\n";

        //Calculate samples needed to finish
        for (Tile<Color> tile : tiles) {
            render_info.samples_needed += tile.getDimensions().x * tile.getDimensions().y * configuration.pixel_samples;
        }

        std::cout << "Need " << render_info.samples_needed << " Samples\n";

        //Create the sampler
        sampler = std::make_unique<BackwardIntegrator>(&bvh, BackwardIntegrator::SceneSamplerConfiguration{
                configuration.max_ray_depth,
                configuration.max_ray_depth_material,
                configuration.no_hit_material
        });

        //Raytrace the scene
        std::vector<std::thread> workers;

        //Thread that updates the stats about the samples completed
        auto updater = std::thread(&Renderer::update, this);
        for (Tile<Color> tile : tiles){
            workers.emplace_back(std::thread(&Renderer::trace_tile, this, tile));
        }
        for (std::thread& worker : workers){
            worker.join();
        }
        render_info.stage = Ended;
        updater.join();
    }

    int& pixelsamples(){
        return configuration.pixel_samples;
    }
    int& maxraydepth(){
        return configuration.max_ray_depth;
    }
    std::shared_ptr<Material>& max_depth_material(){
        return configuration.max_ray_depth_material;
    }
    std::shared_ptr<Material>& no_hit_material(){
        return configuration.no_hit_material;
    }

    void set_updater(std::chrono::milliseconds interval, std::function<void(const RenderInfo&)> function){
        interval = interval;
        updater = function;
    }

private:

    //TODO: Extract logic in object
    void update(){
        //We need to sleep the thread for seconds, or else the fast reads on samples_completed
        //block the other threads (the workers) from executing and we have a slow down
        while(render_info.stage != Ended){
            std::this_thread::sleep_for(interval);
            //Call function
            updater(render_info);
        }
        updater(render_info);
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

                    Color sample_color = sampler->sample(r);

                    //Avoid bad samples killing the pixels
                    for (int k = 0; k < 3; ++k) {if(sample_color[k] != sample_color[k]) sample_color[k] = 0.0f;}
                    pixel_color += sample_color;
                }
                tile(i, j) = pixel_color / static_cast<float>(configuration.pixel_samples);
            }
            //Update samples every line to avoid lock overhead
            render_info.samples_completed += tile_dimensions.x * configuration.pixel_samples;
        }
    }

private:
    std::unique_ptr<BackwardIntegrator> sampler;
    int tile_number = 4;
    Camera camera;
    Node scene;
    BVH bvh;


    std::chrono::milliseconds interval{1000};
    std::function<void(const RenderInfo&)> updater;
};
