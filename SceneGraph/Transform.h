//
// Created by kevin on 15/08/20.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

//The classes that inherit from this can be transformed in a 3D space
//The class stores the inverse of the transform
class Transform {
public:
    Transform() : mTransform(glm::mat4(1.0f)) {}

    void translate(glm::vec3 vector){
        mTransform = glm::translate(mTransform, -vector);
    }
    void scale(glm::vec3 axis){
        mTransform = glm::scale(mTransform, 1.0f/axis);
    }
    void rotate(glm::vec3 axis, float angle){
        mTransform = glm::rotate(mTransform, -angle, axis);
    }

    void clear(){
        mTransform = glm::mat4(1.0f);
    }

public:
    glm::vec3 transform(glm::vec4 vector) const {
        if(vector.w == 0.0){
            return glm::vec3(glm::transpose(mTransform) * vector);
        }
        return glm::vec3(mTransform * vector);
    }

protected:
    glm::mat4 mTransform;

};