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
// - blend: Blend the emitted and scattered
// - scatter: Scatter the ray
class Material {
public:
    //Scatter the ray
    virtual bool scatter(const Intersection& i, Ray& r) const = 0;

    //Get the color of the material at the surface point
    //Override this function to implement effects or
    //to use the ray and intersection point in the blending process
    Color color(const Ray& r, const Intersection& i, const Color& incoming) const {
        Color emitted = this->emitted(r, i);
        return this->blend(emitted, incoming);
    };

    void addTexture(std::string key, std::shared_ptr<Texture> texture){
        textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    }
    void removeTexture(std::string key){
        textures.erase(key);
    }

protected:
    //The emitted color
    virtual Color emitted(const Ray& r, const Intersection& i) const { return Color{0, 0, 0}; }
    //Blend the color of the surface and the color of the incoming light
    virtual Color blend(const Color& emitted,
                        const Color& incoming) const = 0;


protected:
    std::map<std::string, std::shared_ptr<Texture>> textures;

};
