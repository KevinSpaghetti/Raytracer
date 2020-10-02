//
// Created by kevin on 02/10/20.
//

#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

//Represents an Orthonomial base composed by 3 vectors s, t and n
class ONB{
public:
    ONB(){}

    glm::vec3 operator[](int index) const {
        return axis[index];
    }

    glm::vec3 u() const { return axis[0]; }
    glm::vec3 v() const { return axis[1]; }
    glm::vec3 w() const { return axis[2]; }

    glm::vec3 local(float x, float y, float z) const {
        return x*u() + y*v() + z*w();
    }

    glm::vec3 local(const glm::vec3& a) const {
        return a.x*u() + a.y*v() + a.z*w();
    }

    //Build an ONB from a direction n that will become w
    static ONB build_from_w(const glm::vec3& n){
        ONB onb;
        onb.axis[2] = glm::normalize(n);
        glm::vec3 a = (fabs(onb.w().x) > 0.9) ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
        onb.axis[1] = glm::normalize(glm::cross(onb.w(), a));
        onb.axis[0] = glm::cross(onb.w(), onb.v());
        return onb;
    }

private:
    glm::vec3 axis[3];
};
