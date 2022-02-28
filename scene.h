//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_SCENE_H
#define PATHTRACER_SCENE_H
#include "global.h"
#include "intersection.h"
#include "ray.h"
#include "bvh.h"

class Scene {
public:
    int width;
    int height;
    float rr = 0.8;
    float fov = 27.3909;
    std::vector<vec3> frame_buffer;
    std::vector<uint8_t> image;
    std::vector<Object*> objects;
    BVH* bvh;
public:
    Scene() = default;
    Scene(int w, int h);
    void add_obj(Object* obj);
    void build_bvh();
    void render();
    Intersection intersect(const Ray& ray) const;
    void sample_light(Intersection& inter, float& pdf);
    vec3 castRay(const Ray& ray);
};
#endif //PATHTRACER_SCENE_H
