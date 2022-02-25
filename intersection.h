//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_INTERSECTION_H
#define PATHTRACER_INTERSECTION_H
#include "global.h"
#include "material.h"

class Object;

class Intersection {
public:
    bool has;
    vec3 pos = {};
    vec3 normal = {};
    vec3 emit = {};
    Object* obj;
    Material* m;
    float t;
public:
    Intersection() {
        has = false;
        pos = vec3(0.0f);
        normal = vec3(0.0f);
        emit = vec3(0.0f);
        obj = nullptr;
        m = nullptr;
        t = std::numeric_limits<float>::infinity();
    }
};

#endif //PATHTRACER_INTERSECTION_H
