//
// Created by kevin on 15/08/20.
//
#pragma once

#include <memory>
#include <iostream>
#include <list>
#include "../Materials/Material.h"
#include "../Geom/Transform.h"
#include "../Geom/Hittable.h"
#include "../Geom/Intersections.h"
#include "../Mesh/Mesh.h"
#include "../BVH/Boxable.h"
#include "../BVH/AABB.h"
#include "../Mesh/AARectMesh.h"
#include "../Materials/DiffuseLight.h"
#include "ImportanceSamplingEnabled.h"
#include "../Utils/Random.h"
#include "../Mesh/SphereMesh.h"


class Node : public Hittable {
public:
    enum class Type {
        Empty,
        Camera,
        Visual,
        Light
    };

    //Set global and local transforms to the identity transform
    Node() : local(LocalTransform()), global(LocalTransform()) {}

    virtual Type type() const = 0;

    //Since every node can have children the hit function
    //should at least check for the intersections with the children
    //every node needs to check the children because even if the ray does not hit
    //the empty node it can still contain children with meshes that can be hit
    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        for (const auto& child : children) {
            child->hit(r, intersections);
        }
    }

    void add(const std::shared_ptr<Node> child){
        children.push_back(child);
    }
    void remove(const std::shared_ptr<Node> child){
        children.remove(child);
    }

    void translate(const Direction& direction){
        local.translate(direction);
    };
    void rotate(const Normal& axis, const float radians){
        local.rotate(axis, radians);
    };
    void scale(const Dimension& dimensions){
        local.scale(dimensions);
    };

    LocalTransform transform_local() const {
        return local;
    }
    GlobalTransform transform_global() const {
        return global;
    }

    void set_transform_local(const LocalTransform& transform){
        local = transform;
    }
    void set_transform_global(const GlobalTransform& transform){
        global = transform;
    }

    std::list<std::shared_ptr<Node>> getChildren() const {
        return children;
    }

    //Compute all the node's global transforms
    static void computeGlobalTransforms(Node* root, GlobalTransform parent_global = GlobalTransform()){
        root->set_transform_global(GlobalTransform::concat(parent_global, root->transform_local()));
        auto children = root->getChildren();
        for (const auto& child : children) {
            computeGlobalTransforms(child.get(), root->transform_global());
        }
    }

protected:
    //From object space to parent coordinate system
    //Must be set during scene creation
    LocalTransform local;
    //From object space to root coordinate system
    //Can be empty during scene creation, will be set from the
    //renderer
    GlobalTransform global;

    std::list<std::shared_ptr<Node>> children;
};
class EmptyNode : public Node {

    Type type() const override {
        return Type::Empty;
    }
};
class VisualNode : public Node, public Boxable<AABB> {
public:
    VisualNode(const std::shared_ptr<Mesh> mesh, const std::shared_ptr<Material> material)
        : mesh(mesh), material(material) {}

    Type type() const override {
        return Type::Visual;
    }

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        //Apply the transform to the ray
        //The recursion applies the transforms without the need to explicitly multiply the matrices
        Ray t(global.pointToObjectSpace(r.getOrigin()),
              global.directionToObjectSpace(r.getDirection()), r.getType());

        std::array<Intersection, 2> mesh_intersections;
        int n_ins;
        mesh->intersect(t, mesh_intersections, n_ins);
        for(auto is = mesh_intersections.begin(); is != mesh_intersections.begin() + n_ins; ++is){
            //Push back the object with the coords in object space
            ObjectIntersection o{};
            o.point = is->point;
            o.normal = is->normal;
            o.ws_point = global.pointToWorldSpace(is->point);
            o.ws_normal = global.directionToWorldSpace(is->normal);
            o.uv = is->uv;
            o.t = is->t;
            o.isFront = is->isFront;
            o.node = this;
            intersections.push_back(o);
        }

        for (auto child : children) {
            child->hit(r, intersections);
        }

    }

    //Return the bounding box surrounding this node
    //with respect to object space
    AABB getSurroundingBox() const override {
        return mesh->getSurroundingBox();
    }


    Mesh* getMesh() const{
        return mesh.get();
    }
    Material* getMaterial() const{
        return material.get();
    }

protected:
    VisualNode() {}

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};
class LightNode : public VisualNode, public ImportanceSamplingEnabled {
protected:
    LightNode(const std::shared_ptr<Mesh> mesh,const std::shared_ptr<Material> material) : VisualNode(mesh, material) {}

public:
    Type type() const override {
        return Type::Light;
    }

    virtual Ray randomPoint(const Point& origin) const = 0;

};
class AreaLightNode : public LightNode {
public:
    //Area light node is a special type of visual node used in importance sampling
    AreaLightNode(const float width, const float height, const Color color, const float intensity)
        : LightNode(std::make_shared<AARectMesh>(AARectMesh::Axis::XZ, Point{0, 0, 0}, width, height),
                    std::make_shared<DiffuseLight>(color, intensity)),
          width(width),
          height(height),
          center({0, 0, 0}) {}

    Ray randomPoint(const Point& origin) const override {
        auto random_point = Point{randomized::scalar::random(-width/2, width/2),
                                  0,
                                  randomized::scalar::random(-height/2, height/2)};

        //Transform random_point from local space to world space
        random_point = global.pointToWorldSpace(random_point);
        //Set the ray as a shadow ray
        return Ray(origin, glm::normalize(random_point - origin), Ray::Type::Shadow);
    }

    Color random(const Point &origin) const override {
        return {0, 0, 0};
    }


protected:
    float width;
    float height;

    Point center;
};
class SphereLightNode : public LightNode {
public:
    //Area light node is a special type of visual node used in importance sampling
    SphereLightNode(const float radius, const Color color, const float intensity)
            : LightNode(std::make_shared<SphereMesh>(Point{0, 0, 0}, radius),
                        std::make_shared<DiffuseLight>(color, intensity)),
              radius(radius),
              center({0, 0, 0}) {}

    Ray randomPoint(const Point& origin) const override {
        auto random_point = randomized::vector::in_unit_sphere() * radius;
        random_point = global.pointToWorldSpace(random_point);
        //Set the ray as a shadow ray
        return Ray(origin, glm::normalize(random_point - origin), Ray::Type::Shadow);
    }

    Color random(const Point &origin) const override {
        return {0, 0, 0};
    }


protected:
    float radius;

    Point center;
};

class CameraNode : public Node {
public:
    CameraNode(Point lookfrom, Point lookat, Normal up, float vfov, float aspect_ratio, float aperture, float focus_dist) {
        auto theta = glm::radians(vfov);
        auto h = tanf(theta/2.0f);
        float viewport_height = 10.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(up, w));
        v = glm::cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        upper_left_corner = origin - horizontal/2.0f + vertical/2.0f - focus_dist * w;

        lens_radius = aperture / 2;
    }

    Ray get(float s, float t) const {
        //vec3 rd = lens_radius * randomized::vector::in_unit_disk();
        Point offset = {0, 0, 0}; //u * rd.x + v * rd.y;

        const Point r_origin = origin + offset;
        const Point r_direction = upper_left_corner + s*horizontal - t*vertical - origin - offset;
        return Ray(global.pointToWorldSpace(r_origin),
                   global.directionToWorldSpace(r_direction),
                   Ray::Type::Camera);
    }

    Type type() const override {
        return Type::Camera;
    }

private:
    Point origin{};
    Point upper_left_corner{};
    Point horizontal{};
    Point vertical{};
    Point u{}, v{}, w{};
    float lens_radius{};
};
