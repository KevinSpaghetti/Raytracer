//
// Created by kevin on 06/10/20.
//

#pragma once

#include "Material.h"
#include <glm/gtx/compatibility.hpp>

//Implementation of a diffuse + specular brdf
//Based on lambertian diffuse and cook-torrance specular
//TODO: Fix the fireflies
class PBRMaterial : public Material {
public:
    PBRMaterial(Color base_color,
                float roughness) :
                    base_color(base_color),
                    roughness(roughness) {}

    Color f(const Intersection &i, const Ray &wi, const Ray &wo) const override {
        const Direction V = wo.getDirection();
        const Direction L = wi.getDirection();
        const Direction N = i.ws_normal;
        const Direction H = glm::normalize(V+L);

        float NdotL = glm::dot(N, L);
        float NdotV = glm::dot(N, V);

        Color diffuse = base_color / consts::pi;
        float denom = (4 * NdotL * NdotV);
        float alpha = roughness * roughness;

        float F = schlickFresnel(glm::dot(H, V));
        float G = GGX_Geometry(N, H, V, L, alpha);
        float D = GGX_Distribution(N, H, alpha);

        float specular = (G * D) / denom;

        return (1.0f - F) * diffuse + F * specular;
    }

    bool scatters(const Intersection &i, const Ray &wo) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray &wo) const override {
        Point reflected = glm::reflect(-wo.getDirection(), i.ws_normal + roughness * randomized::vector::in_unit_sphere());
        if(roughness > 0.9){
            return Ray(i.ws_point, reflected, Ray::Type::Diffuse);
        }
        return Ray(i.ws_point, reflected, Ray::Type::Specular);
    }

    bool emits(const Intersection &i, const Ray &wo) const override {
        return false;
    }
    Color emit(const Intersection &i, const Ray &wo) const override {
        return Color({0.0, 0.0, 0.0});
    }

private:

    float GGX_Distribution(const Normal& N, const Normal& H, float alpha) const {
        float NdotH = glm::dot(N, H);
        float alpha2 = alpha * alpha;
        float NdotH2 = NdotH * NdotH;
        float denom = NdotH2 * alpha2 + (1.0f - NdotH2);
        if(NdotH > 0.0f){
            NdotH = 1.0f;
        }else{
            NdotH = 0.0f;
        }
        return (NdotH * alpha2) / (consts::pi * denom * denom);
    }
    float GGX_Geometry(const Direction& N, const Direction& H, const Direction& V, const Direction& L, float alpha) const {
        float NdotH = glm::dot(N, H);
        float NdotV = glm::dot(N, V);
        float NdotL = glm::dot(N, L);
        float HdotV = glm::dot(V, H);

        float G1 = (2 * NdotH * NdotV) / HdotV;
        float G2 = (2 * NdotH * NdotL) / HdotV;

        return glm::min(glm::min(1.0f, G1), G2);
    }
    float schlickFresnel(float cosTheta) const {
        return powf(1 - cosTheta, 5);
    }

private:
    Color base_color;
    float roughness;
};
