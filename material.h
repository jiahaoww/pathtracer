//
// Created by jiahaoww on 2022/2/23.
//

#ifndef PATHTRACER_MATERIAL_H
#define PATHTRACER_MATERIAL_H

#include "global.h"
#include <string>

enum MATERIAL_TYPE {
    DIFFUSE,
    MICRO_FACET
};

class Material {
public:
    MATERIAL_TYPE type;
    float roughness;
    float metallic = 0.0;
    float ior;
    float shine_exponent;
    vec3 Kd;
    vec3 Ks;
    vec3 emit;
    std::string map_kd;
    std::string map_ks;
    bool has_emission;
public:
    Material();
    Material(MATERIAL_TYPE t, const vec3& kd, const vec3& ks, const vec3& e, const float& i, const float& shine_exponent = 32.0, std::string  map_kd = "", std::string  map_ks = "");
    vec3 sample(const vec3& N, const vec3& wo) const;
    vec3 eval(const vec3& N, const vec3& wo, const vec3& wi) const;
    float pdf(const vec3& N, const vec3& wo, const vec3& wi) const;
};

#endif //PATHTRACER_MATERIAL_H
