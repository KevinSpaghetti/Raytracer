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
    Transform() : mTransform(glm::mat4(1.0f)),
                  mInverse(glm::mat4(1.0f)),
                  mTranslate(glm::mat4(1.0f)),
                  mScale(glm::mat4(1.0f)),
                  mRotate(glm::mat4(1.0f)) {}

    void translate(glm::vec3 vector){
        mTranslate = glm::translate(glm::mat4(1.0f), vector);
        update();
    }
    void scale(glm::vec3 axis){
        mScale = glm::scale(glm::mat4(1.0f), axis);
        update();
    }
    void rotate(glm::vec3 axis, float radians){
        mRotate = glm::rotate(glm::mat4(1.0f), radians, axis);
        update();
    }

    void clear(){
        mTransform = glm::mat4(1.0f);
        mInverse = glm::mat4(1.0f);
    }

public:
    glm::vec3 pointToObjectSpace(glm::vec3 vector) const {
        return mInverse * glm::vec4(vector, 1.0f);
    }
    glm::vec3 directionToObjectSpace(glm::vec3 vector) const {
        return glm::normalize(glm::transpose(mInverse) * glm::vec4(vector, 0.0f));
    }

    glm::vec3 pointToWorldSpace(glm::vec3 vector) const {
        return mTransform * glm::vec4(vector, 1.0f);
    }
    glm::vec3 directionToWorldSpace(glm::vec3 vector) const {
        return glm::normalize(glm::transpose(mTransform) * glm::vec4(vector, 0.0f));
    }

private:
    void update(){
        mTransform = mTranslate * mRotate * mScale;
        mInverse = glm::inverse(mTransform);
    }

protected:
    glm::mat4 mTranslate;
    glm::mat4 mScale;
    glm::mat4 mRotate;

    glm::mat4 mTransform;
    glm::mat4 mInverse;
};