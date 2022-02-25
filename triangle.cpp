//
// Created by jiahaoww on 2022/2/24.
//

#include "triangle.h"
Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, Material* mtl) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    e1 = v[1] - v[0];
    e2 = v[2] - v[0];
    m = mtl;
    normal = glm::normalize(glm::cross(e1, e2));
    vec3 p_min = {std::min({v[0].x, v[1].x, v[2].x}), std::min({v[0].y, v[1].y, v[2].y}), std::min({v[0].z, v[1].z, v[2].z})};
    vec3 p_max = {std::max({v[0].x, v[1].x, v[2].x}), std::max({v[0].y, v[1].y, v[2].y}), std::max({v[0].z, v[1].z, v[2].z})};
    bounding_box = {p_min, p_max};
}

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec2 t0, vec2 t1, vec2 t2, Material* mtl) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    tex[0] = t0;
    tex[1] = t1;
    tex[2] = t2;
    m = mtl;
    e1 = v[1] - v[0];
    e2 = v[2] - v[0];
    normal = glm::normalize(glm::cross(e1, e2));

    vec3 p_min = {std::min({v[0].x, v[1].x, v[2].x}), std::min({v[0].y, v[1].y, v[2].y}), std::min({v[0].z, v[1].z, v[2].z})};
    vec3 p_max = {std::max({v[0].x, v[1].x, v[2].x}), std::max({v[0].y, v[1].y, v[2].y}), std::max({v[0].z, v[1].z, v[2].z})};
    bounding_box = {p_min, p_max};
}

float Triangle::get_area() {
    return glm::length(glm::cross(e1, e2)) * 0.5f;
}

Intersection Triangle::get_interaction(const Ray &ray) {
    Intersection inter;
    if (glm::dot(ray.dir, normal) > 0.0f) {
        return inter;
    }
    vec3 s = ray.ori - v[0];
    vec3 s1 = glm::cross(ray.dir, e2);
    vec3 s2 = glm::cross(s, e1);
    double s1dote1 = glm::dot(s1, e1);
    if (std::abs(s1dote1) < EPSILON) {
        return inter;
    }
    double inv_s1dote1 = 1.0f / s1dote1;

    double b1 = glm::dot(s1, s) * inv_s1dote1;
    if (b1 < 0 || b1 > 1) {
        return inter;
    }
    double b2 = glm::dot(s2, ray.dir) * inv_s1dote1;
    if (b2 < 0 || b1 + b2 > 1) {
        return inter;
    }

    double t = glm::dot(s2, e2) * inv_s1dote1;
    if (t < 0) {
        return inter;
    }

    inter.has = true;
    inter.normal = normal;
    inter.obj = this;
    inter.m = m;
    inter.pos = ray.ori + ray.dir * (float)t;
    inter.t = t;
    return inter;

}

void Triangle::sample(Intersection& inter, float &pdf) {
    float x1 = std::sqrt(get_random_float());
    float x2 = get_random_float();
    vec3 p =  v[0] * (1.0f - x1) + v[1] * x1 * (1 - x2)  + v[2] * (x1 * x2);
    inter.has = true;
    inter.pos = p;
    inter.m = m;
    inter.normal = normal;
    inter.obj = this;
    inter.emit = m->emit;
    pdf = 1.0f / get_area();
}





