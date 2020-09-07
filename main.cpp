#include <iostream>
#include <glm/glm.hpp>
#include "Geom/Ray.h"
#include "consts.h"
#include "Hittable_List.h"
#include "Utils/Camera.h"
#include "Geom/Sphere.h"
#include "BVH/BVH.h"
#include "SceneGraph/Texture.h"
#include "Geom/Rect.h"
#include <thread>

vec3 random_unit_vector() {
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return vec3(r*cos(a), r*sin(a), z);
}
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1,1);
        if (glm::length(p)*glm::length(p) >= 1) continue;
        return p;
    }
}
vec3 random_in_unit_disk(){
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (glm::length(p)*glm::length(p) >= 1) continue;
        return p;
    }
}

class Diffuse_Light : public Material {
public:
    Diffuse_Light(shared_ptr<Texture> a) : emit(a) {}
    Diffuse_Light(vec4 c) : emit(make_shared<Solid_Color>(c)) {}

    bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        return false;
    }

    vec4 emitted(double u, double v, const vec3& p) const {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<Texture> emit;
};

class Lambertian : public Material {
public:
    Lambertian(const glm::vec4& a) : albedo(make_shared<Solid_Color>(a)) {}
    Lambertian(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(const glm::vec4& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        vec3 reflected = glm::reflect(glm::normalize(r_in.getDirection()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.getDirection(), rec.normal) > 0);
    }

private:
    glm::vec4 albedo;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(float ri) : ref_idx(ri) {}

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec4& attenuation, Ray& scattered) const override {
        attenuation = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        float etai_over_etat = rec.front_face ? (1.0/ref_idx) : ref_idx;
        vec3 unit_direction = glm::normalize(r_in.getDirection());
        vec3 refracted = glm::refract(glm::normalize(r_in.getDirection()), rec.normal, etai_over_etat);
        scattered = Ray(rec.p, refracted);

        return true;
    }

private:
    float ref_idx;
};

glm::vec4 ray_color(Ray& r, const vec4& background, const Hittable& world, int depth){
    if (depth <= 0) return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    hit_record ht;
    if (!world.hit(r, 0.001, infinity, ht)) return background;

    Ray scattered;
    glm::vec4 attenuation;
    vec4 emitted = ht.mat_ptr->emitted(ht.u, ht.v, ht.p);

    if(!ht.mat_ptr->scatter(r, ht, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

hittable_list random_scene() {
    hittable_list world;


    auto checker = make_shared<Checker_Texture>(glm::vec4(0.2, 0.3, 0.1, 1.0), glm::vec4(0.9, 0.9, 0.9, 1.0));
    auto ground_material = make_shared<Lambertian>(checker);
    //world.add(make_shared<Sphere>(vec3(0,-1000,0), 1000, ground_material));

    auto material1 = make_shared<Dielectric>(1.5);
    //world.add(make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(glm::vec4(0.4, 0.2, 0.1, 1.0));
    //world.add(make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(glm::vec4(0.7, 0.6, 0.5, 1.0), 0.0);
    //world.add(make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    world.add(make_shared<Sphere>(vec3(0,-1000,0), 1000, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(vec3(0,2,0), 2, make_shared<Metal>(glm::vec4(0.7, 0.6, 0.5, 1.0), 0.6)));

    auto difflight = make_shared<Diffuse_Light>(vec4(4,4,4,1.0));
    world.add(make_shared<Rect>(3, 5, 1, 3, -2, difflight));

    return world;
}


int main() {

    double aspect_ratio = 16.0f / 9.0f;
    int image_width = 1080;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 4;
    int max_depth = 8;


    std::cout << "P3" << "\n";
    std::cout << image_width << ' ' << image_height << '\n' << 255 << '\n';

    hittable_list world = random_scene();
    //BVH world(worldL, 0, 0);

    auto material_ground = make_shared<Lambertian>(glm::vec4(0.8, 0.8, 0.0, 1.0f));
    auto material_center = make_shared<Lambertian>(glm::vec4(0.7, 0.3, 0.3, 1.0f));
    auto material_left   = make_shared<Metal>(glm::vec4(0.8, 0.8, 0.8, 1.0f), 0.3);
    auto material_right  = make_shared<Metal>(glm::vec4(0.8, 0.6, 0.2, 1.0f), 1.0);

    glm::vec3 lookfrom(26,3,6);
    glm::vec3 lookat(0,2,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    glm::vec4 *colorBuffer = new glm::vec4[image_height * image_width];


    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            glm::vec4 color(0.0, 0.0, 0.0, 1.0f);
            for (int k = 0; k < samples_per_pixel; ++k) {
                auto u = (float(i) + random_double()) / (image_width-1);
                auto v = (float(j) + random_double()) / (image_height-1);
                Ray r = cam.get_ray(u, v);
                color += ray_color(r, vec4(0, 0, 0, 0), world ,max_depth);
            }
            color /= static_cast<float>(samples_per_pixel);
            color = sqrt(color);

            colorBuffer[(image_height-j-1)*image_width + i] = color;
        }
        std::cerr << "\rCompleted " << (image_height-j) << "/" << image_height << ' ' << std::flush;
    }

    for (int i = 0; i < image_width * image_height; ++i) {
        std::cout << static_cast<int>(clamp(colorBuffer[i].r * 255.99f, 0, 255)) << ' '
                  << static_cast<int>(clamp(colorBuffer[i].g * 255.99f, 0, 255)) << ' '
                  << static_cast<int>(clamp(colorBuffer[i].b * 255.99f, 0, 255)) << '\n';
    }
    

    delete[](colorBuffer);

    std::cerr << '\n';
    return 0;
}
