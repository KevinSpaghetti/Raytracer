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
#include "Camera.h"
#include "Geom/Intersection.h"
#include "Utils/Consts.h"
#include "Materials/VNMaterial.h"
#include "Utils/Random.h"

class Renderer {
public:
    //Renderer configuration
    struct Configuration {
        int pixel_samples = 1;
        int max_ray_depth = 1;
        bool backface_culling = false;
    };

    Renderer(Configuration configuration) : configuration(configuration) {}

    //For now render only color
    void render(Node graph, Buffer<Color>& target, Camera camera){
        //Divide the buffer in tiles
        //int tile_number = 1;
        std::vector<Tile<Color>> tiles = Tile<Color>::split(target, tile_number);
        int n_tile = 1;
        for (Tile<Color> t : tiles) {
            std::cout << n_tile << " tile\n"
                      << "\t Position ( " << t.getStartx() << " , " << t.getStarty() << ")\n"
                      << "\t Size     ( " << t.getWidth()  << " , " << t.getHeight() << ")\n";
            ++n_tile;
        }

        //Copy the scene graph and apply all the transforms
        scene = graph;

        //Calculate samples needed to finish
        for (Tile<Color> tile : tiles) {
            samples_needed += tile.getHeight() * tile.getWidth();
        }

        std::cout << "Need " << samples_needed << " Samples\n";
        //Raytrace the scene
        std::vector<std::thread> workers;

        //Thread that updates the stats about the samples completed
        workers.emplace_back(std::thread(&Renderer::update_stats, this));
        for (Tile<Color> tile : tiles){
            workers.emplace_back(std::thread(&Renderer::trace_tile, this, tile));
        }

        for (std::thread& worker : workers){
            worker.join();
        }

    }

private:

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

        auto origin = glm::vec3(0, 0, 4);
        auto horizontal = vec3(10.0, 0, 0);
        auto vertical = vec3(0, 10.0, 0);
        auto upper_left_corner = origin - horizontal/2.0f + vertical/2.0f - vec3(0, 0, 4.0);

        for (int i = 0; i < tile.getHeight(); ++i) {
            for (int j = 0; j < tile.getWidth(); ++j) {
                auto v = float(tile.getStarty() + i) / (tile.getHeight()*tile_number);
                auto h = float(tile.getStartx() + j) / (tile.getWidth()*tile_number);

                Ray r = Ray(origin, (upper_left_corner + h*horizontal - v*vertical) - origin);
                Color color = trace_ray(r, configuration.max_ray_depth);
                tile(i, j) = color;
            }
            //Update samples every line to avoid lock overhead
            samples_completed += tile.getWidth();
        }
    }

    //TODO: Add no hit shader and max depth reached shader
    Color trace_ray(Ray r, int ray_depth) {

        if(ray_depth <= 0) {
            return Color{0.0, 0.0, 0.0};
        }

        std::list<ObjectIntersection> intersections = scene.hit(r);
        if (intersections.empty()){
            glm::vec3 dir = glm::normalize(r.getDirection());
            float t = 0.5f * (dir.y + 1.0);
            return (1.0f-t) * Color{1.0, 1.0, 1.0} + t*Color{0.5, 0.7, 1.0};
        }

        //TODO: find the closes intersection to the camera plane
        ObjectIntersection i = intersections.front();
        Color emitted = i.material->emitted(r, i);
        Color scattered = i.material->scatter(r, i);
        //Get scattered rays
        Ray scatter = Ray(i.pv, i.pn + randomized::vector::in_unit_sphere());
        Color traced = trace_ray(scatter, ray_depth - 1);
        return 0.5f * (i.pn + glm::vec3{1.0, 1.0, 1.0});

    }


private:
    std::atomic<int> samples_completed = 0;
    int samples_needed = 0;
    int tile_number = 2;
    Node scene;
    Configuration configuration;

};
