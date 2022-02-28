//
// Created by vr on 2022/2/28.
//

#ifndef PATHTRACER_CAMERA_H
#define PATHTRACER_CAMERA_H
#include "global.h"

class Camera {
public:
    vec3 eye;
    vec3 look_at;
    vec3 up;
    float fov;
    int width;
    int height;
public:
    Camera() = default;
    Camera(vec3 e, vec3 center, vec3 world_up, float fov, int w, int h): eye(e), look_at(center), up(world_up), fov(fov), width(w), height(h) {}
    mat4 get_view_matrix() {
        return glm::lookAt(eye, look_at, up);
    }
};

#endif //PATHTRACER_CAMERA_H
