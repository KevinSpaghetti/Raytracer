//
// Created by kevin on 13/08/20.
//
#pragma once
#include <glm/glm.hpp>
#include <map>
#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"
#include "../Textures/ImageTexture.h"

//The function emitted and scattered can be called multiple times
//To create a material override
// - emitted: The emitted light
// - scattered: The scattered light
// - scatter: Scatter the ray
class Material {
public:
    //Scatter the ray
    virtual bool scatter(const Intersection& i, Ray& r) const = 0;

    //Get the color of the material at the surface point
    //Override this function to implement effects or
    //to use the ray and intersection point in the blending process
    virtual Color color(const Intersection& i, const Ray& r, const Color& incoming) const = 0;

    void addTexture(std::string key, std::shared_ptr<Texture> texture){
        textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    }
    void removeTexture(std::string key){
        textures.erase(key);
    }


protected:
    std::map<std::string, std::shared_ptr<Texture>> textures;

};
