//
// Created by kevin on 20/08/20.
//

#pragma once

#include <optional>
#include <thread>
#include "SceneGraph/Node.h"
#include "Utils/Buffer.h"
#include "Utils/Tile.h"
#include "Camera.h"
#include "Geom/Intersection.h"
#include "Utils/Consts.h"

class Renderer {
public:
    //Renderer configuration
    struct Configuration {
        int pixel_samples = 1;
        int max_ray_depth = 1;
    };

    Renderer(Configuration configuration) : configuration(configuration) {}

    //For now render only color
    void render(Node graph, Buffer<Color>& target, Camera camera){
        //Divide the buffer in tiles
        int tile_number = 2;
        std::vector<Tile<Color>> tiles = Tile<Color>::split(target, tile_number);
        int n_tile = 1;
        for (Tile<Color> t : tiles) {
            std::cout << n_tile << " tile\n"
                      << "\t Position ( " << t.getStartx() << " , " << t.getStarty() << ")\n"
                      << "\t Size     ( " << t.getWidth()  << " , " << t.getHeight() << ")\n";
            ++n_tile;
        }

        //Scene graph
        scene = graph;

        //Raytrace the scene
        const int multithreading = 1;
        if(multithreading == 0){
            n_tile = 1;
            for (Tile<Color> t : tiles){
                std::cout << "Tracing tile " << n_tile << "\n";
                raytrace(t, camera);
                ++n_tile;
            }
        }else{
            std::vector<std::thread> tile_workers;
            for (Tile t : tiles) {
                tile_workers.push_back(std::thread(&Renderer::raytrace, this, t ,camera));
            }

            for (std::thread& worker : tile_workers){
                worker.join();
            }
        }
    }

private:
    void raytrace(Tile<Color> tile, Camera camera){
        for (int i = 0; i < tile.getHeight(); ++i) {
            for (int j = 0; j < tile.getWidth(); ++j) {
                auto u = float(i) / (tile.getWidth()-1);
                auto v = float(j) / (tile.getHeight()-1);

                Ray r = camera.get_ray(u, v);
                glm::vec3 color = trace_ray(r, configuration.max_ray_depth);
                tile(i, j) = Color{color.x, color.y, color.z};
            }
        }
    }

    glm::vec3 trace_ray(Ray r, int ray_depth) {
        if(ray_depth <= 0) {
            return glm::vec3{0, 0, 0};
        }

        std::list<Intersection> intersections = scene.hit(r, 0.001, infinity);
        if (intersections.empty()) return glm::vec3{0, 0, 0};

        //find the closes intersection to the camera plane
        Intersection closest = intersections.front();
        for (Intersection i : intersections) {
            if(i.pv.y <= closest.pv.y){
                closest = i;
            }
        }

        glm::vec3 target = closest.pv.vec() + closest.pn.vec();
        Ray casted(closest.pv.vec(), target - closest.pn.vec());
        return 0.5f * trace_ray(casted, ray_depth - 1);
    }


private:
    Node scene;
    Configuration configuration;

};
