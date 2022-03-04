//
// Created by jiahaoww on 2022/3/3.
//

#include <thread>
#include <future>
#include "renderer.h"
#include "stb/stb_image_write.h"

Renderer::Renderer(Scene &s):scene(s) {
    frame_buffer.resize(scene.width * scene.height);
    image.resize(scene.width * scene.height * 3);
}

void Renderer::render(int spp) {
    int width = scene.width;
    int height = scene.height;
    Camera camera = scene.camera;
    float scale = tan(camera.fov * 0.5 * PI / 180.0);
    float imageAspectRatio = width / (float) height;
    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 inverse_view = glm::inverse(view);

    // change the spp value to change sample amount
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < height; ++j) {
        int threads = std::thread::hardware_concurrency() << 1;
        int offset = width / threads;
        std::vector<std::future<void>> futures;

        for (uint32_t i = 0; i < threads; ++i) {
            int begin_offset = offset * i;
            int end_offset = i + 1 == threads ? width : begin_offset + offset;

            std::future<void> task = std::async(std::launch::async, [&](int begin_offset, int end_offset) {
                for (int i = begin_offset; i < end_offset; ++i) {
                    // generate primary ray direction
                    for (int k = 0; k < spp; k++) {
                        float x = (2 * (i + get_random_float()) / (float)width - 1) *
                                  imageAspectRatio * scale;
                        float y = (1 - 2 * (j + get_random_float()) / (float)height) * scale;

                        vec3 dir = glm::normalize(inverse_view * vec4(vec3(x, y, -1.0f), 0.0f));
                        frame_buffer[j * width + i] += glm::clamp(scene.castRay(Ray(camera.eye, dir), 0), 0.0f, 1.0f) / (float)spp;
                    }
                }
            }, begin_offset, end_offset);
            futures.emplace_back(std::move(task));
        }
        UpdateProgress(j / (float) height);
    }
    UpdateProgress(1.f);


}

void Renderer::write_to_file(const std::string& filename) {
    int width = scene.width;
    int height = scene.height;
    const float gamma = 1.0 / 2.2;
    for (auto i = 0; i < height * width; ++i) {
        image[3 * i] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].x), gamma));
        image[3 * i + 1] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].y), gamma));
        image[3 * i + 2] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].z), gamma));

    }
    stbi_write_png(filename.c_str(), width, height, 3, image.data(), 0);
}
