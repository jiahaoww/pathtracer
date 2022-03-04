//
// Created by vr on 2022/3/2.
//

#include "sphere.h"

Sphere::Sphere(vec3 c, float r): center(c), radius(r) {
    vec3 v_min = c - vec3(r);
    vec3 v_max = c + vec3(r);
    bounding_box = {v_min, v_max};
}

float Sphere::get_area() {
    return 4 * PI * radius * radius;
}

void Sphere::sample(Intersection &inter, float &pdf) {
    float theta = get_random_float() * PI;
    float phi = get_random_float() * PI * 2.0f;
    vec3 dir = {std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta)};
    inter.m = m;
    inter.normal = dir;
    inter.emit = inter.m->emit;
    inter.obj = this;
    inter.pos = center + radius * dir;
    pdf = 1.0f / get_area();
}

Intersection Sphere::get_intersection(const Ray &ray) {

    Intersection inter;
    float a = glm::dot(ray.dir, ray.dir);
    float b = 2.0f * glm::dot(ray.ori - center, ray.dir);
    float c = glm::dot(ray.ori - center, ray.ori - center) - radius * radius;
    float delta = b * b - 4.0f * a * c;
    if (delta < 0.0f) {
        return inter;
    } else {
        float t1, t2;
        float t;
        if (delta == 0) {
            t = -b / (2.0f * a);
        } else {
            t1 = (-b - std::sqrt(delta)) / (2.0f * a);
            t2 = (-b + std::sqrt(delta)) / (2.0f * a);
            if (t1 > t2) {
                std::swap(t1, t2);
            }
            t = t1;
            if (t1 < 0.0f) {
                t = t2;
            }
        }
        if (t < 0.1f)
            return inter;
        inter.pos = ray.ori + t * ray.dir;
        inter.has = true;
        inter.m = m;
        inter.normal = glm::normalize(inter.pos - center);
        inter.obj = this;
        inter.t = t;
        return inter;

    }
}


