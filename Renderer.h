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
#include "Materials/VNMaterial.h"

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

        //Raytrace the scene
        std::vector<std::thread> workers;

        for (Tile<Color> tile : tiles){
            workers.emplace_back(std::thread(&Renderer::trace_tile, this, tile));
        }

        for (std::thread& worker : workers){
            worker.join();
        }

    }

private:

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
                glm::vec3 color = trace_ray(r, configuration.max_ray_depth);
                tile(i, j) = Color{color.x, color.y, color.z};
            }
        }
    }

    //TODO: Add no hit shader and max depth reached shader
    glm::vec3 trace_ray(Ray r, int ray_depth) {

        if(ray_depth <= 0) {
            return glm::vec3{1.0, 0, 0};
        }

        std::list<ObjectIntersection> intersections = scene.hit(r);
        if (intersections.empty()) return glm::vec3{0.2, 0.2, 0.2}; //Return background color

        //TODO: find the closes intersection to the camera plane
        ObjectIntersection closest = intersections.front();
        return closest.material->scatter(r, closest);

    }


private:
    int tile_number = 4;
    Node scene;
    Configuration configuration;

};
