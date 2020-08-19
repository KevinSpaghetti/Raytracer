//
// Created by kevin on 17/08/20.
//

#pragma once

#include <glm/glm.hpp>

class BaseMaterial {
public:

    virtual glm::vec4 emitted() const = 0;
    virtual bool scatter() const = 0;

private:

};