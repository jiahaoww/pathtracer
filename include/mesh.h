//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_MESH_H
#define PATHTRACER_MESH_H
#include "object.h"
#include "bvh.h"
class Mesh: public Object {
public:
    std::vector<Object*> prims;
    BVH* bvh;
public:
    explicit Mesh(const std::vector<Object*>& objects);
    float get_area() override;
    Intersection get_intersection(const Ray& ray) override;
    void sample(Intersection& inter, float& pdf) override;
};

#endif //PATHTRACER_MESH_H
