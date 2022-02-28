#ifndef PATHTRACER_TEXTURE_H
#define PATHTRACER_TEXTURE_H

#include <vector>

#include <glm/glm.hpp>

class Texture {
    std::vector<glm::u8vec3> image;
public:
    int width, height;

    explicit Texture(const char *filename);

    const glm::u8vec3 *operator[](size_t row) const noexcept;

    glm::u8vec3 *operator[](size_t row) noexcept;

    void write(const char *filename) const;
};

#endif //PATHTRACER_TEXTURE_H
