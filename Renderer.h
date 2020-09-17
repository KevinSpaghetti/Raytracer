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
    void render(Node graph, Buffer<Color>& target, Camera cam){
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

        //Save the camera
        camera = cam;

        //Save the scene and build the bvh
        scene = graph;
        std::cout << "Building the bvh\n";
        bvh = BVH(graph);
        std::cout << "Done\n";

        //Calculate samples needed to finish
        for (Tile<Color> tile : tiles) {
            samples_needed += tile.getHeight() * tile.getWidth() * configuration.pixel_samples;
        }

        std::cout << "Need " << samples_needed << " Samples\n";
        //Raytrace the scene
        std::vector<std::thread> workers;

        Ray test({-1.76467013, -0.00251743244, -0.558851004}, {-0.70301044, -0.00447021239, -0.711165487});
        Color i = trace_ray(test, 0);

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
        for (int i = 0; i < tile.getHeight(); ++i) {
            for (int j = 0; j < tile.getWidth(); ++j) {
                Color pixel_color{0, 0, 0};
                for (int sample = 0; sample < configuration.pixel_samples; ++sample) {
                    Color sample_color;
                    auto v = float(tile.getStarty() + i + randomized::scalar::random()) / (tile.getHeight() * tile_number);
                    auto h = float(tile.getStartx() + j + randomized::scalar::random()) / (tile.getWidth() * tile_number);

                    //Ray r = Ray(origin, (upper_left_corner + h * horizontal - v * vertical) - origin);
                    Ray r = camera.get_ray(h, v);

                    sample_color = trace_ray(r, 0);

                    pixel_color += sample_color;
                }
                tile(i, j) = pixel_color / static_cast<float>(configuration.pixel_samples);
            }
            //Update samples every line to avoid lock overhead
            samples_completed += tile.getWidth() * configuration.pixel_samples;
        }
    }

    //TODO: Add no hit materials and max depth reached materials
    //TODO: Remember the last BVH or Scene node hit and
    //      check that first
    Color trace_ray(Ray r, int ray_depth) {

        //If the ray has reached max depth then call the max depth shader
        if (ray_depth > configuration.max_ray_depth) {
            return maxRayDepthReached(r); //Return a background material color
        }

        //Check intersections with the scene
        std::list<ObjectIntersection> intersections;
        bvh.hit(r, intersections);

        //If there are no intersections call the no hit shader
        if (intersections.empty()) {
            return noIntersections(r);
        }

        //If backface culling is activated pick the intersection
        //with a face that is front
        ObjectIntersection intersection;
        if (configuration.backface_culling) {
            intersection = intersections.front();
            while (intersections.empty() || intersection.isFront == true) {
                intersections.pop_front();
            }
            if (intersections.empty()) {
                noIntersections(r);
            }
        } else {
            intersection = intersections.front();
            for (ObjectIntersection i : intersections) {
                if (glm::length(r.getOrigin() - i.pv) < glm::length(r.getOrigin() - intersection.pv)) {
                    intersection = i;
                }
            }
        }


        //Resolve the material color
        std::shared_ptr<Material> material = intersection.node->getMaterial();
        Ray ray({0, 0, 0}, {0, 0, 0});
        Color incoming{0, 0, 0};
        if (material->scatter(intersection, r, ray)) {
            incoming = trace_ray(ray, ray_depth + 1);
        }

        return material->color(intersection, r, incoming);
    }

protected:

    Color maxRayDepthReached(const Ray& r) const {
        return Color{1.0, 0, 0};
    }

    Color noIntersections(const Ray& r) const {
        Point unit_direction = r.getDirection();
        float t = 0.5f * (unit_direction.y + 1.0);
        return (1.0f - t) * Color(1.0) + t * Color{0.5, 0.7, 1.0};
    }

private:

    std::atomic<int> samples_completed = 0;
    int samples_needed = 0;
    int tile_number = 1;
    Camera camera;
    Node scene;
    BVH bvh;
    Configuration configuration;
};
