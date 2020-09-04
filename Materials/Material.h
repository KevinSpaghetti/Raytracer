//
// Created by kevin on 13/08/20.
//
#pragma once
#include <glm/glm.hpp>
#include <map>
#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"
#include "ImageTexture.h"


class Material {
public:
    virtual Color emitted(const Ray& r, const Intersection& p) const {
        return Color{0, 0, 0};
    }
    virtual Color scatter(const Ray& r, const Intersection& i) const = 0;

    void addTexture(std::string key, std::shared_ptr<Texture> texture){
        textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    }
    void removeTexture(std::string key){
        textures.erase(key);
    }

protected:
    std::map<std::string, std::shared_ptr<Texture>> textures;
};
