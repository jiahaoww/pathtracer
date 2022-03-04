//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_OBJECT_H
#define PATHTRACER_OBJECT_H
#include "material.h"
#include "intersection.h"
#include "ray.h"
#include "aabb.h"

class Object {
public:
    Material* m = nullptr;
    AABB bounding_box;
    Object() {
        m = nullptr;
    }
    virtual float get_area() = 0;
    virtual void sample(Intersection& inter, float& pdf) = 0;
    virtual Intersection get_intersection(const Ray& ray) = 0;
};
#endif //PATHTRACER_OBJECT_H
