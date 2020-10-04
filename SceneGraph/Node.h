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
    VisualNode() = delete;

    VisualNode(const std::shared_ptr<Mesh> mesh, const std::shared_ptr<Material> material)
        : mesh(mesh), material(material) {}

    Type type() const override {
        return Type::Visual;
    }

    void hit(const Ray& r, std::vector<ObjectIntersection>& intersections) const override {

        //Apply the transform to the ray
        //The recursion applies the transforms without the need to explicitly multiply the matrices
        Ray t(global.pointToObjectSpace(r.getOrigin()),
              global.directionToObjectSpace(r.getDirection()));

        std::vector<Intersection> mesh_intersections;
        mesh->intersect(t, mesh_intersections);
        for(Intersection& is : mesh_intersections){
            //Push back the object with the coords in object space
            is.pv = global.pointToWorldSpace(is.pv);
            is.pn = global.directionToWorldSpace(is.pn);
            intersections.push_back({is, this});
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
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};
class AreaLightNode : public VisualNode, public ImportanceSamplingEnabled {

    //Area light node is a special type of visual node used in importance sampling
    AreaLightNode(const float width, const float height, const Color color, const float intensity)
        : VisualNode(std::make_shared<AARectMesh>(AARectMesh::Axis::XZ, Point{0, 0, 0}, width, height), std::make_shared<DiffuseLight>(color, intensity)),
          width(width),
          height(height),
          center({0, 0, 0}) {}

    Type type() const override {
        return Type::Light;
    }

    //Define functions for importance sampling
    //Importance sampling is a LightNode responsibility
    virtual double pdf(const Point& origin, const Normal& direction) const override {
        std::vector<Intersection> ins;
        //This does not work since we need to sample if the ray intersects something in the scene,
        //doing it like this only checks if the ray hits the light with direction and origin
        mesh->intersect(Ray(origin, direction), ins);
        if (ins.empty()) return 0.0f;

        Intersection i = ins.front();

        //Doesn't work because we need to transform the light to world space
        float area = width * height;
        float distance_squared = glm::length(i.pv - origin) * glm::length(i.pv - origin) * glm::length(direction);
        float cosine = fabs(glm::dot(direction, i.pn) / glm::length(direction));

        return distance_squared / (cosine * area);
    }

    virtual Point random(const Point& o) const override {
        auto random_point = Point{randomized::scalar::random(-width/2, width/2),
                                  0,
                                  randomized::scalar::random(-height/2, height/2)};
        return center + random_point;
    }
protected:
    float width;
    float height;

    Point center;
};
//TODO: Integrate the position and rotation of the camera into the get function
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
        return Ray(origin + offset, upper_left_corner + s*horizontal - t*vertical - origin - offset);
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
