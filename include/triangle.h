//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_TRIANGLE_H
#define PATHTRACER_TRIANGLE_H
#include "object.h"
#include "aabb.h"

class Triangle: public Object{
public:
    vec3 v[3];
    vec3 vn[3];
    vec2 tex[3];
    vec3 normal;
    vec3 e1;
    vec3 e2;
public:
    Triangle() = default;
    Triangle(vec3 v0, vec3 v1, vec3 v2, Material* m);
    Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec2 t0, vec2 t1, vec2 t2, Material *mtl);
    float get_area() override;
    Intersection get_intersection(const Ray& ray) override;
    void sample(Intersection& inter, float& pdf) override;
};

#endif //PATHTRACER_TRIANGLE_H
