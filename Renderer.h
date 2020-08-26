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
        int tile_number = 1;
        std::vector<Tile<Color>> tiles = Tile<Color>::split(target, tile_number);
        int n_tile = 1;
        for (Tile<Color> t : tiles) {
            std::cout << n_tile << " tile\n"
                      << "\t Position ( " << t.getStartx() << " , " << t.getStarty() << ")\n"
                      << "\t Size     ( " << t.getWidth()  << " , " << t.getHeight() << ")\n";
            ++n_tile;
        }

        //Copy the scene graph and apply all the transforms
        scene = graph.transform();


        //Raytrace the scene
        n_tile = 1;

        auto origin = glm::vec3(0, 0, 1);
        auto horizontal = vec3(2.0, 0, 0);
        auto vertical = vec3(0, 2.0, 0);
        auto upper_left_corner = origin - horizontal/2.0f + vertical/2.0f - vec3(0, 0, 1.0);

        for (Tile<Color> tile : tiles){
            std::cout << "Tracing tile " << n_tile << "\n";
            for (int i = 0; i < tile.getHeight(); ++i) {
                for (int j = 0; j < tile.getWidth(); ++j) {
                    auto u = float(tile.getStarty() + i) / (tile.getHeight()*tile_number);
                    auto v = float(tile.getStartx() + j) / (tile.getWidth()*tile_number);

                    Ray r = Ray(origin, upper_left_corner + u*horizontal - v*vertical - origin);
                    glm::vec3 color = trace_ray(r, configuration.max_ray_depth);
                    tile(j, i) = Color{color.x, color.y, color.z};
                }
            }
            ++n_tile;
        }

    }

private:

    glm::vec3 trace_ray(Ray r, int ray_depth) {

        if(ray_depth <= 0) {
            return glm::vec3{1.0, 0, 0};
        }

        std::list<Intersection> intersections = scene.hit(r, 0.001, infinity);
        if (intersections.empty()) return glm::vec3{0, 0, 0}; //Return background color

        //TODO: find the closes intersection to the camera plane
        Intersection closest = intersections.front();
        std::shared_ptr<Material> m = closest.material;
        glm::vec3 c = closest.material->scatter(r, closest);
        return c;

    }


private:
    Node scene;
    Configuration configuration;

};
