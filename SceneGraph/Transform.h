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
    Transform() : mTransform(glm::mat4(1.0f)), mInverse(glm::mat4(1.0f)) {}

    void translate(glm::vec3 vector){
        mTransform = glm::translate(mTransform, -vector);
        mInverse = glm::translate(mInverse, vector);
    }
    void scale(glm::vec3 axis){
        mTransform = glm::scale(mTransform, 1.0f/axis);
        mInverse = glm::scale(mInverse, axis);
    }
    void rotate(glm::vec3 axis, float radians){
        mTransform = glm::inverse(glm::rotate(mTransform, radians, axis));
        mInverse = glm::rotate(mInverse, radians, axis);
    }

    void clear(){
        mTransform = glm::mat4(1.0f);
        mInverse = glm::mat4(1.0f);
    }

public:
    glm::vec3 pointToObjectSpace(glm::vec3 vector) const {
        return mTransform * glm::vec4(vector, 1.0f);
    }
    glm::vec3 directionToObjectSpace(glm::vec3 vector) const {
        return glm::transpose(mTransform) * glm::vec4(vector, 0.0f);
    }

    glm::vec3 pointToWorldSpace(glm::vec3 vector) const {
        return mInverse * glm::vec4(vector, 1.0f);
    }
    glm::vec3 directionToWorldSpace(glm::vec3 vector) const {
        return glm::transpose(mInverse) * glm::vec4(vector, 0.0f);
    }

protected:
    glm::mat4 mTransform;
    glm::mat4 mInverse;
};