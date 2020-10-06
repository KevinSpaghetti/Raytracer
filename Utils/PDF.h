//
// Created by kevin on 02/10/20.
//

#pragma once

#include <bits/shared_ptr.h>
#include "Structs.h"
#include "../Geom/ONB.h"
#include "Consts.h"
#include "Random.h"
#include "../Geom/Hittable.h"

class PDF {
public:
    virtual float value(const Direction& direction) const = 0;
    virtual Direction generate() const = 0;

    virtual ~PDF() = default;
};

class CosinePDF : public PDF {
public:
    CosinePDF(const Normal& w){
        uvw = ONB::build_from_w(w);
    }

    float value(const Direction &direction) const override {
        float cosine = glm::dot(glm::normalize(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine/consts::pi;
    }

    Direction generate() const override {
        return uvw.local(randomized::vector::in_cosine_direction());
    }

private:
    ONB uvw;
};

class ZeroPDF : public PDF {
    float value(const Direction &direction) const override {
        return 0.0f;
    }

    Direction generate() const override {
        return Direction{0, 0, 0};
    }
};

class MixPDF : public PDF {
public:
    MixPDF(PDF* p0, PDF* p1) : p0(p0), p1(p1){}

    float value(const Direction &direction) const override {
        return 0.5f * p0->value(direction) + 0.5f * p1->value(direction);
    }

    Direction generate() const override {
        if(randomized::scalar::random() < 0.5){
            return p0->generate();
        }else{
            return p1->generate();
        }
    }

private:
    PDF* p0;
    PDF* p1;
};