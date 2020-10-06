//
// Created by kevin on 04/09/20.
//

#pragma once

#include <random>
#include "glm/glm.hpp"

namespace randomized {

    namespace scalar {
        static float random() {
            static thread_local std::mt19937 generator;
            std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
            return distribution(generator);
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
        static glm::vec3 unit_vector() {
            auto a = randomized::scalar::random(0, 2*consts::pi);
            auto z = randomized::scalar::random(-1, 1);
            auto r = sqrt(1 - z*z);
            return glm::vec3{r*cos(a), r*sin(a), z};
        }
        static glm::vec3 in_unit_disk(){
            while (true) {
                auto p = glm::vec3{randomized::scalar::random(-1,1), randomized::scalar::random(-1,1), 0};
                if (pow(glm::length(p),2) >= 1) continue;
                return p;
            }
        }
        static glm::vec3 in_unit_hemisphere(const glm::vec3 normal){
            glm::vec3 p = in_unit_sphere();
            if (glm::dot(normal, p) > 0.0f) {
                return glm::normalize(p);
            }else{
                return glm::normalize(-p);
            }
        }
        static glm::vec3 in_cosine_direction() {
            float r1 = randomized::scalar::random();
            float r2 = randomized::scalar::random();
            auto z = sqrtf(1.0f-r2);

            auto phi = 2*consts::pi*r1;
            auto x = cos(phi)*sqrt(r2);
            auto y = sin(phi)*sqrt(r2);

            return glm::vec3(x, y, z);
        }
    }

};
