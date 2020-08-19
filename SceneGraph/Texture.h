//
// Created by kevin on 15/08/20.
//

#ifndef RAYTRACER_TEXTURE_H
#define RAYTRACER_TEXTURE_H

#include "../consts.h"

using glm::vec4;

class Texture {
public:
    virtual vec4 value(double u, double v, const vec3& p) const = 0;
};

class Solid_Color : public Texture {
public:
    Solid_Color() {}
    Solid_Color(vec4 c) : color_value(c) {}

    Solid_Color(double red, double green, double blue)
            : Solid_Color(vec4(red,green,blue,1.0f)) {}

    virtual vec4 value(double u, double v, const vec3& p) const override {
        return color_value;
    }

private:
    vec4 color_value;
};

class Checker_Texture : public Texture {
public:
    Checker_Texture() {}

    Checker_Texture(shared_ptr<Texture> t0, shared_ptr<Texture> t1)
            : even(t0), odd(t1) {}

    Checker_Texture(vec4 c1, vec4 c2)
            : even(make_shared<Solid_Color>(c1)) , odd(make_shared<Solid_Color>(c2)) {}

    virtual vec4 value(double u, double v, const vec3& p) const override {
        auto sines = sin(10*p.x)*sin(10*p.y)*sin(10*p.z);
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<Texture> odd;
    shared_ptr<Texture> even;
};
#endif //RAYTRACER_TEXTURE_H
