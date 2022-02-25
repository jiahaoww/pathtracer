//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_RAY_H
#define PATHTRACER_RAY_H
#include "global.h"

class Ray {
public:
    vec3 ori = {};
    vec3 dir = {};
    float min_t;
public:
    Ray() {
        min_t = 0.0f;
        ori = vec3(0.0f);
        ori = vec3(0.0f);
    }
    Ray(vec3 o, vec3 d, float t = 0.0f) {
        ori = o;
        dir = d;
        min_t = t;
    }
};

#endif //PATHTRACER_RAY_H
