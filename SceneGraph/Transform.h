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

    void move(glm::vec3 vector){
        vecTranslation = vector;
    }
    void scale(glm::vec3 vector){
        matScale = glm::scale(vector);
    }
    void rotate(glm::vec3 axis, float angle){
        matRotation = glm::rotate(angle, axis);
    }

    void clear(){
        vecTranslation = {0, 0, 0};
        matRotation = glm::mat3();
        matScale = glm::mat3();
    }

    glm::vec3 apply(glm::vec3 vector){
        return vecTranslation + (matRotation * matScale * vector);
    }

protected:
    glm::vec3 vecTranslation;
    glm::mat3 matRotation;
    glm::mat3 matScale;

};