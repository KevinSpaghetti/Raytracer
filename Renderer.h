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
#include "Geom/Intersection.h"
#include "Utils/Consts.h"
#include "Utils/Random.h"
#include "BVH/BVH.h"
#include "Geom/ObjectIntersection.h"
#include "Integrators/BackwardIntegrator.h"
#include "Integrators/NormalDebugIntegrator.h"
#include "SceneGraph/SceneList.h"
#include "Integrators/DebugIntegrator.h"
#include "Samplers/RandomSampler.h"
#include "Samplers/StratifiedSampling.h"
#include "SceneGraph/VisualNode.h"
#include "SceneGraph/LightNode.h"
#include "SceneGraph/CameraNode.h"
#include "Samplers/ConstantSampler.h"

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
    enum class RenderStage {
        NotStarted,
        Started,
        Ended
    };
    struct RenderInfo {
        std::atomic<int> samples_completed = 0;
        int samples_needed = 0;
        RenderStage stage = RenderStage::NotStarted;
    } render_info;
public:

    //For now render only color
    //cam must be a pointer to CameraNode in the scene
    void render(std::shared_ptr<Node> graph, Buffer<Color>& target, CameraNode* from_camera){
        render_info.stage = RenderStage::Started;
        //Save the camera
        active_camera = from_camera;
        //Save the scene, process the graph and build the bvh
        scene = graph;

        //Process scene graph

        //Compute the global transforms for every node
        Node::computeGlobalTransforms(scene.get());
        //Now that we have all the nodes in the scene in world space we need to:
        //  Find and collect all the lights to perform importance sampling
        //  Find and collect all the geometry and the lights to trace the scene
        //Then we can start importance sampling by having
        //  A vector of lights and their positions
        //  An acceleration structure that we can sample for the nodes and the lights
        std::vector<LightNode*> lights;
        std::vector<VisualNode*> visuals;

        collectLights(scene.get(), lights);
        collectVisuals(scene.get(), visuals);

        bvh = std::make_shared<BVH>(visuals);

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
        std::cout << "Done\n";

        //Calculate samples needed to finish
        for (Tile<Color> tile : tiles) {
            render_info.samples_needed += tile.getDimensions().x * tile.getDimensions().y * configuration.pixel_samples;
        }

        std::cout << "Need " << render_info.samples_needed << " Samples\n";

        //Create the sampler
        sampler = std::make_unique<BackwardIntegrator>(bvh.get(), BackwardIntegrator::SceneSamplerConfiguration{
                configuration.max_ray_depth,
                configuration.max_ray_depth_material,
                configuration.no_hit_material
        });

        //Raytrace the scene
        std::vector<std::thread> workers;
        workers.reserve(tiles.size());

        //Thread that updates the stats about the samples completed
        auto updater_thread = std::thread(&Renderer::update, this);
        for (const auto& tile : tiles){
            workers.emplace_back(std::thread(&Renderer::render_tile, this, tile));
        }
        for (std::thread& worker : workers){
            worker.join();
        }
        render_info.stage = RenderStage::Ended;
        updater_thread.join();
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

    void set_updater(const std::chrono::milliseconds interval, std::function<void(const RenderInfo&)> function){
        this->interval = interval;
        updater = std::move(function);
    }

private:

    void collectLights(Node *node, std::vector<LightNode*>& lights){
        if(node->type() == Node::Type::Light){
            auto* light = dynamic_cast<LightNode*>(node);
            lights.push_back(light);
        }
        for(const auto& child : node->getChildren()){
            collectLights(child.get(), lights);
        }
    }
    void collectVisuals(Node *node, std::vector<VisualNode*>& visuals){
        if(node->type() == Node::Type::Visual || node->type() == Node::Type::Light){
            auto* visual = dynamic_cast<VisualNode*>(node);
            visuals.push_back(visual);
        }
        for(const auto& child : node->getChildren()){
            collectVisuals(child.get(), visuals);
        }
    }

    void update(){
        //We need to sleep the thread for seconds, or else the fast reads on samples_completed
        //block the other threads (the workers) from executing and we have a slow down
        while(render_info.stage != RenderStage::Ended){
            std::this_thread::sleep_for(interval);
            //Call function
            updater(render_info);
        }
        //Call once when the render is over
        updater(render_info);
    }

    void render_tile(Tile<Color> tile){
        const Point2D tile_dimensions = tile.getDimensions(); //dimensions of the tile in pixels
        const Point2D pixel_dimensions = 1.0f / (tile.getDimensions() * static_cast<float>(tile_number));

        //Needs to be local to the tile since samplers can have internal state
        auto pixel_sampler{std::make_unique<StratifiedJitteredSampler>(configuration.pixel_samples)};
        for (int i = 0; i < static_cast<int>(tile_dimensions.y); ++i) {
            for (int j = 0; j < static_cast<int>(tile_dimensions.x); ++j) {
                Color pixel_color{0, 0, 0};
                Point2D pixel_upper_left_corner = (tile.getStart() + Point2D{j, i});
                for (int sample = 0; sample < configuration.pixel_samples; ++sample) {
                    Point2D sampling_coords = pixel_sampler->generateSample(sample); //Generate different sample coords base on different coordinates
                    Point2D sample_coords = (pixel_upper_left_corner + sampling_coords) * pixel_dimensions;
                    Ray r = active_camera->get(sample_coords.x, sample_coords.y); //Get the ray that from the camera goes through the pixel

                    Color sample_color = glm::clamp(sampler->sample(r), 0.0f, 1.0f);

                    //Avoid bad samples killing the pixels (NaN != NaN) in float standard
                    for (int k = 0; k < 3; ++k) {if(sample_color[k] != sample_color[k]) sample_color[k] = 0.0f;}
                    pixel_color += sample_color;
                }
                tile(i, j) = pixel_color / static_cast<float>(configuration.pixel_samples);
            }
            //Update samples every line to avoid lock overhead
            render_info.samples_completed += static_cast<int>(tile_dimensions.x * configuration.pixel_samples);
        }
    }

private:
    std::unique_ptr<BackwardIntegrator> sampler;
    int tile_number = 4;
    CameraNode *active_camera;
    std::shared_ptr<Node> scene;
    std::shared_ptr<BVH> bvh;


    std::chrono::milliseconds interval{1000};
    std::function<void(const RenderInfo&)> updater;
};
