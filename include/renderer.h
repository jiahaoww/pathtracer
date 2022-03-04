//
// Created by jiahaoww on 2022/3/3.
//

#ifndef PATHTRACER_RENDERER_H
#define PATHTRACER_RENDERER_H
#include "scene.h"

class Renderer {
public:
    Scene scene;
    std::vector<vec3> frame_buffer;
    std::vector<uint8_t> image;
public:
    Renderer(Scene& s);
    void render(int spp);
    void write_to_file(const std::string& filename);
};

#endif //PATHTRACER_RENDERER_H
