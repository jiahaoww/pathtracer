//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_SCENE_H
#define PATHTRACER_SCENE_H
#include "global.h"
#include "intersection.h"
#include "ray.h"
#include "bvh.h"
#include "camera.h"
#include "EmbreeAccelerator.h"

class Scene {
public:
    int width;
    int height;
    float rr = 0.8;
    std::vector<Object*> objects;
    BVH* bvh;
    Camera camera;
    EmbreeAccelerator* acc;

public:
    Scene() = default;
    explicit Scene(Camera& cam);
    void add_obj(Object* obj);
    void build_bvh();
    Intersection intersect(const Ray& ray) const;
    void sample_light(Intersection& inter, float& pdf);
    vec3 castRay(const Ray& ray);
    vec3 castRay_merge(const Ray& ray);
};
#endif //PATHTRACER_SCENE_H
