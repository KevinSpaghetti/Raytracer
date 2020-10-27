//
// Created by kevin on 15/08/20.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

//The classes that inherit from this can be transformed in a 3D space
//The class stores the inverse of the transform for efficiency reasons

//The GlobalTransform class represents a non decomposable global transform matrix
//the operations get applied in the order in which they are specified
class GlobalTransform {
public:
    GlobalTransform() : mTransform(glm::mat4(1.0f)),
                        mInverse(glm::mat4(1.0f)) {}

    //Clear all the transforms applied
    virtual void clear(){
        mTransform = glm::mat4(1.0f);
        mInverse = glm::mat4(1.0f);
    }

    static GlobalTransform concat(const GlobalTransform& t1, const GlobalTransform& t2){
        GlobalTransform t;
        t.mTransform = t1.mTransform * t2.mTransform;
        t.mInverse = glm::inverse(t.mTransform);
        return t;
    }

    glm::vec3 pointToWorldSpace(glm::vec3 vector) const {
        glm::vec4 result = mTransform * glm::vec4(vector, 1.0f);
        return result;
    }
    glm::vec3 directionToWorldSpace(glm::vec3 vector) const {
        glm::vec4 result = glm::transpose(mTransform) * glm::vec4(vector, 0.0f);
        return glm::normalize(result);
    }
    glm::vec3 pointToObjectSpace(glm::vec3 vector) const {
        return glm::vec4(vector, 1.0f) * glm::transpose(mInverse);
    }
    glm::vec3 directionToObjectSpace(glm::vec3 vector) const {
        glm::vec4 result = glm::vec4(vector, 0.0f) * glm::transpose(mInverse);
        return glm::normalize(result);
    }

private:
    void update(){
        mInverse = glm::inverse(mTransform);
    }

protected:
    glm::mat4 mTransform;
    glm::mat4 mInverse;
};


//Represents a Local Transform composed of a scale, a translation and a rotation,
//the operations are applied in SRT order (scale, rotation and then translation)
//The matrix is decomposed in the Scale, Rotation, and Translation matrices
class LocalTransform : public GlobalTransform {
public:
    LocalTransform() : GlobalTransform(),
                  mTranslate(glm::mat4(1.0f)),
                  mScale(glm::mat4(1.0f)),
                  mRotate(glm::mat4(1.0f)),
                  mTranslateInverse(glm::mat4(1.0f)),
                  mRotateInverse(glm::mat4(1.0f)),
                  mScaleInverse(glm::mat4(1.0f)){}

    void translate(glm::vec3 vector) {
        mTranslate = glm::translate(glm::mat4(1.0f), vector);
        mTranslateInverse = glm::translate(glm::mat4(1.0f), -vector);
        update();
    }
    void scale(glm::vec3 axis) {
        mScale = glm::scale(glm::mat4(1.0f), glm::max(axis, glm::vec3{0.0001, 0.0001, 0.0001}));
        mScaleInverse = glm::scale(1.0f / glm::max(axis, glm::vec3{0.0001, 0.0001, 0.0001}));
        update();
    }
    void rotate(glm::vec3 axis, float radians){
        mRotate = glm::rotate(glm::mat4(1.0f), radians, axis);
        mRotateInverse = glm::rotate(glm::mat4(1.0f), -radians, axis);
        update();
    }

    void clear() override {
        mTransform = glm::mat4(1.0f);
        mInverse = glm::mat4(1.0f);
    }

    glm::mat4 get_translation(){
        return mTranslate;
    }
    glm::mat4 get_scale(){
        return mScale;
    }
    glm::mat4 get_rotation(){
        return mRotate;
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

    glm::mat4 mTranslateInverse;
    glm::mat4 mScaleInverse;
    glm::mat4 mRotateInverse;
};