//
// Created by kevin on 15/08/20.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transform {
public:
    Transform() : mTransform(glm::mat4(1.0f)) {}

    void translate(glm::vec3 vector){
        mTransform = glm::translate(mTransform, -vector);
    }
    void scale(glm::vec3 axis){
        mTransform = glm::scale(mTransform, -axis);
    }
    void rotate(glm::vec3 axis, float angle){
        mTransform = glm::rotate(mTransform, -angle, -axis);
    }

    void clear(){
        mTransform = glm::mat4(1.0f);
    }

public:
    glm::vec3 apply(glm::vec4 vector){
        return glm::vec3(mTransform * vector);
    }

protected:
    glm::mat4 mTransform;

};