//
// Created by kevin on 13/08/20.
//
#pragma once
#include <glm/glm.hpp>
#include <map>
#include "../Geom/Ray.h"
#include "../Geom/Intersection.h"
#include "../Textures/ImageTexture.h"
#include "PropsBasedObject.h"
#include "../Utils/PDF.h"


class Material : public PropsBasedObject {
public:

    //wo: direction of outgoing light (from the point to the eye)
    //wi: direction of incoming light (from emission point to surface)


    virtual Color f(const Intersection &i, const Ray& wi, const Ray& wo) const = 0;

    //Return true if the ray scatters
    virtual bool scatters(const Intersection &i, const Ray& wo) const = 0;
    //Return a scattered ray
    virtual Ray scatter(const Intersection& i, const Ray& wo) const = 0;

    virtual bool emits(const Intersection& i, const Ray& wo) const = 0;
    virtual Color emit(const Intersection& i, const Ray& wo) const = 0;
};
