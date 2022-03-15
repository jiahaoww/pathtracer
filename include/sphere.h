//
// Created by jiahaoww on 2022/3/2.
//

#ifndef PATHTRACER_SPHERE_H
#define PATHTRACER_SPHERE_H

#include "object.h"
#include "intersection.h"

class Sphere: public Object {
public:
    vec3 center;
    float radius;
public:
    Sphere(vec3 c, float r);
    float get_area() override ;
    void sample(Intersection& inter, float& pdf) override;
    Intersection get_intersection(const Ray& ray) override;
};

#endif //PATHTRACER_SPHERE_H
