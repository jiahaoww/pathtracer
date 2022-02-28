#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

Texture::Texture(const char *filename) {
    int channel;
    auto *data = reinterpret_cast<glm::u8vec3 *>(stbi_load(filename, &width, &height, &channel, 0));

    if (data == nullptr) {
        return;
    }

    if (channel != 3) {
        return;
    }

    image.resize(width * height);
    for (int i = 0; i < width * height; ++i) {
        image[i] = data[i];
    }
    stbi_image_free(data);
}

const glm::u8vec3 *Texture::operator[](size_t row) const noexcept {
    return image.data() + row * width;
}

glm::u8vec3 *Texture::operator[](size_t row) noexcept {
    return image.data() + row * width;
}

void Texture::write(const char *filename) const {
    if (!image.empty()) {
        stbi_write_png(filename, width, height, 3, image.data(), 0);
    }
}
