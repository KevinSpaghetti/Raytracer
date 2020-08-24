//
// Created by kevin on 15/08/20.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transform {
public:
    Transform() {}

    void translate(glm::vec3 vector){
        mTransform = glm::translate(mTransform, vector);
    }
    void scale(glm::vec3 axis){
        mTransform = glm::scale(mTransform, axis);
    }
    void rotate(glm::vec3 axis, float angle){
        mTransform = glm::rotate(mTransform, angle, axis);
    }

    void clear(){
        mTransform = glm::mat4();
    }

    glm::vec3 apply(glm::vec4 vector){
        return glm::vec3(mTransform * vector);
    }

    //Apply the transform so that this transform gets applied
    //before t transform
    void add(Transform t){
        mTransform = t.mTransform * mTransform;
    }

protected:
    glm::mat4 mTransform;

};