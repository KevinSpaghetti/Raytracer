//
// Created by kevin on 05/10/20.
//

#pragma once

#include <string>
#include <map>
#include <memory>
#include "../Textures/Texture.h"

class PropsBasedObject {
public:
    PropsBasedObject() : textures() {}

    void addTexture(std::string key, std::shared_ptr<Texture> texture){
        textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    }
    void removeTexture(std::string key){
        textures.erase(key);
    }

protected:
    std::map<std::string, std::shared_ptr<Texture>> textures;
};