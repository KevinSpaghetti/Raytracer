//
// Created by kevin on 04/09/20.
//

#pragma once

#include "glm/glm.hpp"

namespace randomized {

    namespace scalar {
        static float random(){
            return rand() / (RAND_MAX + 1.0);
        }

        static float random(float min, float max){
            return min + (max-min) * randomized::scalar::random();
        }
    }

    namespace vector {
        static glm::vec3 random() {
            return glm::vec3{randomized::scalar::random(), randomized::scalar::random(), randomized::scalar::random()};
        }
        static glm::vec3 random(float min, float max) {
            return glm::vec3{randomized::scalar::random(min,max), randomized::scalar::random(min,max), randomized::scalar::random(min,max)};
        }
        static glm::vec3 in_unit_sphere() {
            while (true) {
                glm::vec3 p = random(-1,1);
                if (pow(glm::length(p),2) >= 1) continue;
                return p;
            }
        }
        static glm::vec3 in_unit_disk(){
            while (true) {
                auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
                if (pow(glm::length(p),2) >= 1) continue;
                return p;
            }
        }
    }

};
