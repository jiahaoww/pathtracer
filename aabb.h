//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_AABB_H
#define PATHTRACER_AABB_H
#include <algorithm>
#include "global.h"
#include "ray.h"
class AABB {
public:
    vec3 v_min{};
    vec3 v_max{};
public:
    AABB() {
        v_min = vec3(std::numeric_limits<float>::infinity());
        v_max = vec3(-std::numeric_limits<float>::infinity());
    }
    AABB(const vec3& v0, const vec3& v1) {
        v_min = v0;
        v_max = v1;
    }
    explicit AABB(const vec3& v) {
        v_min = v_max = v;
    }
    vec3 center() const {
        return (v_min + v_max) / 2.0f;
    }

    vec3 extent() const {
        return v_max - v_min;
    }

    bool intersect(const Ray& ray) const {
        bool is_positive[] = {ray.dir.x > 0.0f, ray.dir.y > 0.0f, ray.dir.z > 0.0f};
        vec3 inv_dir = {1.0f / ray.dir.x, 1.0f / ray.dir.y, 1.0f / ray.dir.z};
        vec3 t1 = (v_min - ray.ori) * inv_dir;
        vec3 t2 = (v_max - ray.ori) * inv_dir;
        float t1_x = t1.x;
        float t1_y = t1.y;
        float t1_z = t1.z;
        float t2_x = t2.x;
        float t2_y = t2.y;
        float t2_z = t2.z;
        if (!is_positive[0]) {
            std::swap(t1_x, t2_x);
        }
        if (!is_positive[1]) {
            std::swap(t1_y, t2_y);
        }
        if (!is_positive[2]) {
            std::swap(t1_z, t2_z);
        }
        float t_enter = std::max({t1_x, t1_y, t1_z});
        float t_exit = std::min({t2_x, t2_y, t2_z});
        return t_enter <= t_exit && t_exit >= 0.0f;
    }

    void Union(const AABB& aabb) {
        vec3 p_min = {std::min(v_min.x, aabb.v_min.x), std::min(v_min.y, aabb.v_min.y), std::min(v_min.z, aabb.v_min.z)};
        vec3 p_max = {std::max(v_max.x, aabb.v_max.x), std::max(v_max.y, aabb.v_max.y), std::max(v_max.z, aabb.v_max.z)};
        v_min = p_min;
        v_max = p_max;
    }

    static AABB Union(const AABB& box1, const AABB& box2) {
        vec3 p_min = {std::min(box1.v_min.x, box2.v_min.x), std::min(box1.v_min.y, box2.v_min.y), std::min(box1.v_min.z, box2.v_min.z)};
        vec3 p_max = {std::max(box1.v_max.x, box2.v_max.x), std::max(box1.v_max.y, box2.v_max.y), std::max(box1.v_max.z, box2.v_max.z)};
        return {p_min, p_max};
    }
};

#endif //PATHTRACER_AABB_H
