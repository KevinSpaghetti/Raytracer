//
// Created by kevin on 06/10/20.
//

#pragma once

#include "Material.h"
#include <glm/gtx/compatibility.hpp>

//Implementation of a diffuse + specular brdf
//Based on lambertian diffuse and cook-torrance specular
//http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
class PBRMaterial : public Material {
public:
    PBRMaterial(Color base_color,
                float roughness,
                float ior) :
                    base_color(base_color),
                    roughness(roughness),
                    ior(ior){}

    Color f(const Intersection &i, const Ray &wi, const Ray &wo) const override {
        const Direction V = wo.getDirection();
        const Direction L = wi.getDirection();
        const Direction N = glm::normalize(i.ws_normal);
        const Direction H = glm::normalize(V+L);

        float NdotL = glm::clamp(glm::dot(N, L), 0.0f, 1.0f);
        float NdotV = glm::clamp(glm::dot(N, V), 0.0f, 1.0f);
        float NdotH = glm::clamp(glm::dot(N, H), 0.0f, 1.0f);

        if(NdotL < 0.0){
            return Color{0, 0, 0};
        }

        Color F0 = glm::abs((Point(1.0) - ior) / (Point(1.0) + ior));
        F0 = F0 * F0;
        F0 = glm::lerp(base_color, F0, roughness); //1.0f - roughness is the "metalness"

        float alpha = roughness * roughness;

        Color F = schlickFresnel(glm::dot(H, V), F0);
        float G = GGX_Geometry(N, H, V, L, alpha);

        float cosT = glm::clamp(glm::dot(wi.getDirection(), glm::normalize(i.ws_normal)), 0.0f, 0.9f);
        Color diffuse = (Point(1.0f) - F) * roughness * base_color;
        float denom = glm::clamp((4 * glm::abs(NdotH) * glm::abs(NdotV)), 0.0f, 1.0f);
        float sinT = sqrt(1.0f - cosT * cosT);
        Color specular = F * G * sinT / denom;

        return (diffuse + specular);
    }

    bool scatters(const Intersection &i, const Ray &wo) const override {
        return true;
    }
    Ray scatter(const Intersection &i, const Ray &wo) const override {
        //Specular sampling strategy
        //Point reflected = glm::reflect(-wo.getDirection(), i.ws_normal);
        //Diffuse sampling strategy
        //Point reflected = randomized::vector::in_unit_hemisphere(i.ws_normal);
        //Sampling the GGX
        //Generate the spherical coords
        float alpha = roughness * roughness;
        float phi = randomized::scalar::random(0, 2.0f * consts::pi); //uniform sampling of the azimuth angle
        float epsilon = randomized::scalar::random();
        float theta = glm::atan(alpha*sqrt(epsilon) / sqrt(1.0f - epsilon));

        Direction supp = (fabs(i.ws_normal.x > 0.9)) ? Direction{0, 1, 0} : Direction{1, 0, 0};
        Direction axis1 = glm::normalize(glm::cross(i.ws_normal, supp));
        Direction axis2 = glm::cross(i.ws_normal, axis1);

        Point cot = glm::cross(i.ws_normal, wo.getDirection());
        Point t   = glm::cross(cot, i.ws_normal);

        Point halfway = axis1 * glm::sin(theta) * glm::cos(phi) +
                        i.ws_normal * glm::cos(theta) +
                        axis2 * glm::sin(theta) * glm::sin(phi);

        Direction generated = glm::reflect(-wo.getDirection(), glm::normalize(halfway));

        return Ray(i.ws_point, generated, Ray::Type::Specular);
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
    float GGX_p(Point V, Point N, Point H, float alpha) const {
        float NdotV = glm::max(glm::dot(N, V), 0.000001f);
        float VdotH = glm::max(glm::dot(V, H), 0.000001f);

        float chi = 0.0;
        if(VdotH/NdotV > 0.0f){
            chi = 1.0f;
        }else{
            chi = 0.0f;
        }
        float VdotH2 = VdotH * VdotH;
        float tan2 = ( 1 - VdotH2 ) / VdotH2;
        return (chi * 2) / (1 + sqrt( 1 + alpha * alpha * tan2 ) );
    }
    float GGX_Geometry(const Direction& N, const Direction& H, const Direction& V, const Direction& L, float alpha) const {
        return GGX_p(V, H, N, alpha)*GGX_p(L, H, N, alpha);
    }
    Color schlickFresnel(float cosTheta, Point F0) const {
        return F0 + (1.0f - F0) * powf(1 - cosTheta, 5.0f);
    }

private:
    Color base_color;
    float roughness;
    float ior;
};
