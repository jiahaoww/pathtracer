//
// Created by jiahaoww on 2022/2/23.
//

#ifndef PATHTRACER_GLOBAL_H
#define PATHTRACER_GLOBAL_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <random>
#include <iostream>

const float PI = std::acos(-1.0f);

const float EPSILON = 1e-5;

using vec3 = glm::vec3;
using vec2 = glm::vec2;

inline float get_random_float()
{
    static thread_local std::random_device dev;
    static thread_local std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(dev);
}

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

inline float clamp(const float &lo, const float &hi, const float &v)
{ return std::max(lo, std::min(hi, v)); }

#endif //PATHTRACER_GLOBAL_H
