//
// Created by Kevin on 25/09/2020.
//

#pragma once

#include "Material.h"

class DiffuseLight : public Material {
public:
    DiffuseLight(const Color color, const float intensity) : c(color), intensity(intensity){}

    Color f(const Intersection &i, const Ray& wi, const Ray& wo) const override {
        //The light only emits, the rendering equation second term is always 0
        return {0.0, 0.0, 0.0};
    }

    bool scatters(const Intersection &i, const Ray& wo) const override {
        return false;
    }
    Ray scatter(const Intersection &i, const Ray& wo) const override {
        return Ray();
    }

    bool emits(const Intersection& i, const Ray& wo) const override {
        return true;
    };
    Color emit(const Intersection& i, const Ray& wo) const override {
        if(glm::dot(i.ws_normal, wo.getDirection()) > 0.0){
            return c * intensity / i.t;
        }else{
            return {0, 0, 0};
        }
    };

private:
    Color c;
    float intensity;
};
