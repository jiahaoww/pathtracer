//
// Created by jiahaoww on 2022/2/23.
//

#include "material.h"
#include <utility>

vec3 reflect(const vec3 &N, const vec3 &dir) {
    vec3 n = N;
    if (glm::dot(N, dir) < 0.0f) {
        n = -n;
    }
    return 2.0f * glm::dot(n, dir) * n - dir;
}

vec3 refract(const vec3 &N, const vec3 &dir, const float &ior) {
    float cos_theta = glm::dot(N, dir);
    float ior1 = 1, ior2 = ior;
    vec3 n = N;
    if (cos_theta < 0) {
        cos_theta = -cos_theta;
        std::swap(ior1, ior2);
        n = -N;
    }
    float eta = ior1 / ior2;
    float k = 1 - eta * eta * (1 - cos_theta * cos_theta);
    vec3 wt(0.0f);
    if (k > 0) {
        wt = normalize(-eta * dir + (eta * cos_theta - sqrtf(k)) * n);
    }
    return wt;
}

float fresnel(const vec3 &N, const vec3 &wo, float ior) {
    float ior1 = 1.0f;
    float ior2 = ior;
    float cos_theta1 = glm::dot(wo, N);
    if (cos_theta1 < 0.0f) {
        std::swap(ior1, ior2);
        cos_theta1 = -cos_theta1;
    }

    float sin_theta1 = std::sqrt(1.0f - cos_theta1 * cos_theta1);
    float sin_theta2 = sin_theta1 * ior1 / ior2;
    if (sin_theta2 > 1.0f) {
        return 1.0;
    }
    float cos_theta2 = std::sqrt(1.0f - sin_theta2 * sin_theta2);
    float r1 = (ior1 * cos_theta1 - ior2 * cos_theta2) / (ior1 * cos_theta1 + ior2 * cos_theta2);
    float r2 = (ior1 * cos_theta2 - ior2 * cos_theta1) / (ior1 * cos_theta2 + ior2 * cos_theta1);
    return (r1 * r1 + r2 * r2) / 2.0f;
}

vec3 toWorld(const vec3 &a, const vec3 &N) {
    vec3 B, C;
    if (std::fabs(N.x) > std::fabs(N.y)) {
        float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
        C = vec3(N.z * invLen, 0.0f, -N.x * invLen);
    } else {
        float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
        C = vec3(0.0f, N.z * invLen, -N.y * invLen);
    }
    B = glm::cross(C, N);
    return a.x * B + a.y * C + a.z * N;
}

float G_schlick_ggx(const vec3 &N, const vec3 &v, const float &k) {
    float dot_nv = std::max(glm::dot(N, v), 0.0f);
    return dot_nv / ((1 - k) * dot_nv + k);
}

Material::Material() : Kd(vec3(0.0f)), Ks(vec3(1.0f)), emit(vec3(0.0f)), ior(1.85f), shine_exponent(32.0f) {
    has_emission = glm::length(emit) >= EPSILON;
    is_specular = glm::length(Ks) >= EPSILON;
    type = MATERIAL_TYPE::DIFFUSE;
}

Material::Material(MATERIAL_TYPE t, const vec3 &kd, const vec3 &ks, const vec3 &e, const float &i,
                   const float &shine_exponent,
                   std::string map_kd, std::string map_ks) : Kd(kd), Ks(ks), emit(e), ior(i),
                                                             shine_exponent(shine_exponent), map_kd(std::move(map_kd)),
                                                             map_ks(std::move(map_ks)) {
    type = t;
    has_emission = glm::length(emit) >= EPSILON;
    is_specular = glm::length(Ks) >= EPSILON;
}

vec3 Material::sample(const vec3 &N, const vec3 &wo) const {
    switch (type) {
        case MATERIAL_TYPE::DIFFUSE: {
            float x1 = get_random_float();
            float x2 = get_random_float();
            float z = std::abs(1.0f - 2.0f * x1);
            float r = std::sqrt(1.0f - z * z);
            float phi = x2 * 2.0f * PI;
            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            vec3 local_wi = {x, y, z};
            vec3 world_wi = toWorld(local_wi, N);
            return world_wi;
        }
        case MATERIAL_TYPE::PHONG: {
            float x1 = get_random_float();
            float x2 = get_random_float();
            float theta = std::acos(std::pow(x1, 1.0f / (shine_exponent + 1.0f)));
            float t = std::pow(x1, 2.0f / (shine_exponent + 1.0f));
            float z = std::sqrt(t);
            float r = std::sqrt(1.0f - z * z);
            float phi = x2 * 2.0f * PI;
            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            vec3 reflect_dir = reflect(N, wo);
            vec3 local_wi = {x, y, z};
            vec3 world_wi = toWorld(local_wi, reflect_dir);
            return world_wi;
        }
        case MATERIAL_TYPE::MICRO_FACET: {
            float x1 = get_random_float();
            float x2 = get_random_float();
            float a = roughness * roughness;
            float a2 = a * a;
                float theta = std::acos(std::sqrt((1.0f - x1) / (x1 * (a2 - 1.0f) + 1.0f)));
            float phi = x2 * 2.0f * PI;

            float z = std::cos(theta);
            float r = std::sqrt(1.0f - z * z);
            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            vec3 local_h = {x, y, z};
            vec3 world_h = toWorld(local_h, N);
            vec3 wi = reflect(world_h, wo);
            return wi;
        }
        case MATERIAL_TYPE::MIRROR: {
            return reflect(N, wo);
        }
        case MATERIAL_TYPE::GLASS: {
            float f = fresnel(N, wo, ior);
            float p = get_random_float();
            if (p <= f) {
                return reflect(N, wo);
            } else {
                return refract(N, wo, ior);
            }
        }
    }
}

vec3 Material::eval(const vec3 &N, const vec3 &wo, const vec3 &wi, const vec3 &KD) const {
    vec3 my_Kd = KD;
    if (texture != nullptr) {
        my_Kd = sdr2ldr(KD);
    }
    vec3 my_Ks = sdr2ldr(Ks);
    switch (type) {
        case MATERIAL_TYPE::DIFFUSE: {
            if (glm::dot(wi, N) > 0.0f) {
                vec3 diffuse = my_Kd;
                vec3 color = diffuse;
                return color / PI;
            } else {
                return {0.0f, 0.0f, 0.0f};
            }
        }
        case MATERIAL_TYPE::PHONG: {
            if (glm::dot(wi, N) > 0.0f) {
                vec3 diffuse = my_Kd / PI;
                vec3 reflect_dir = reflect(N, wo);
                float cos_phi = glm::dot(wi, reflect_dir);
                vec3 specular = my_Ks * std::pow(cos_phi, shine_exponent) * (shine_exponent + 2.0f) / (2.0f * PI);
                return diffuse + specular;
            } else {
                return {0.0f, 0.0f, 0.0f};
            }
        }
        case MATERIAL_TYPE::MICRO_FACET: {
            float cos1 = std::max(glm::dot(N, wi), 0.0f);
            float f_lambert = 1.0f / PI;
            vec3 h = glm::normalize(wo + wi);
            // calculate D
            float a = roughness * roughness;
            float a2 = a * a;
            float cos2 = std::max(glm::dot(N, h), 0.0f);
            float p = cos2 * cos2 * (a2 - 1.0f) + 1.0f;
            float D = a2 / (PI * p * p);

            // calculate G
            float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
            float G = G_schlick_ggx(N, wo, k) * G_schlick_ggx(N, wi, k);
            vec3 F0(0.04);
            F0 = F0 * (1 - metallic) + metallic * my_Kd;
            vec3 F;
            //TODO F = fresnel(N, wo, ior);
            F = F0 + std::pow(1.0f - cos1, 5.0f) * (vec3(1.0f) - F0);

            vec3 f_cook_torrance =
                    D * F * G / (4 * std::max(glm::dot(wo, N), 0.0f) * std::max(glm::dot(wi, N), 0.0f) + EPSILON);
            vec3 _kd = (vec3(1.0f) - F) * (1 - metallic);

            if (glm::dot(N, wi) > 0.0f) {
                return _kd * my_Kd * f_lambert + my_Ks * f_cook_torrance;
            } else {
                return vec3(0.0f);
            }
        }
        case MATERIAL_TYPE::MIRROR: {
            vec3 wr = reflect(N, wo);
            if (wr == wi) {
                return Ks / std::max(glm::dot(wi, N), EPSILON);
            } else {
                return vec3(0.0f);
            }
        }
        case MATERIAL_TYPE::GLASS: {
            float f = fresnel(N, wo, ior);

            if (wi == reflect(N, wo)) {
                return my_Ks * f / std::max(glm::dot(wi, N), EPSILON);
            }
            if (wi == refract(N, wo, ior)) {
                return my_Ks * (1.0f - f) / std::max(std::abs(glm::dot(wi, N)), EPSILON);
            }
            return vec3(0.0f);
        }
    }
}

float Material::pdf(const vec3 &N, const vec3 &wo, const vec3 &wi) const {

    switch (type) {
        case MATERIAL_TYPE::DIFFUSE: {
            if (glm::dot(wi, N) > 0.0f) {
                return 0.5f / PI;
            } else {
                return EPSILON;
            }
        }
        case MATERIAL_TYPE::PHONG: {
            if (glm::dot(wi, N) > 0.0f) {
                vec3 reflect_dir = reflect(N, wo);
                float cos_phi = glm::dot(reflect_dir, wi);
                return std::pow(cos_phi, shine_exponent) * (shine_exponent + 1.0f) / (2.0f * PI) + EPSILON;
            } else {
                return EPSILON;
            }
        }
        case MATERIAL_TYPE::MICRO_FACET: {
            if (glm::dot(wi, N) > 0.0f) {
                vec3 h = glm::normalize(wo + wi);
                float a = roughness * roughness;
                float a2 = a * a;
                float cos_theta = std::max(glm::dot(h, N), EPSILON);
                float exp = (a2 - 1.0f) * cos_theta * cos_theta + 1.0f;
                float D = a2 / (PI * exp * exp);
                return D * cos_theta / (4.0f * std::max(glm::dot(wo, h), 0.0f) + EPSILON);
            } else {
                return EPSILON;
            }
        }
        case MATERIAL_TYPE::MIRROR: {
            vec3 wr = reflect(N, wo);
            if (wr == wi) {
                return 1.0f;
            } else {
                return EPSILON;
            }
        }
        case MATERIAL_TYPE::GLASS: {
            float f = fresnel(N, wo, ior);
            if (wi == reflect(N, wo)) {
                return std::max(f, EPSILON);
            }
            if (wi == refract(N, wo, ior)) {
                return std::max(1.0f - f, EPSILON);
            }
            return EPSILON;
        }

    }
}

vec3 Material::sample_f(const vec3 &N, const vec3 &wo, vec3 &wi, float &pdf, const vec3 &KD) const {
    vec3 my_Kd = KD;
    if (texture != nullptr) {
        my_Kd = sdr2ldr(KD);
    }
    vec3 my_Ks = Ks;
    switch (type) {
        case MATERIAL_TYPE::DIFFUSE: {
            float x1 = get_random_float();
            float x2 = get_random_float();
            float z = std::abs(1.0f - 2.0f * x1);
            float r = std::sqrt(1.0f - z * z);
            float phi = x2 * 2.0f * PI;
            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            vec3 local_wi = {x, y, z};
            vec3 world_wi = toWorld(local_wi, N);
            // set wi
            wi = world_wi;
            // set pdf
            if (glm::dot(wi, N) > 0.0f) {
                pdf = 0.5f / PI;
                return my_Kd / PI;
            } else {
                pdf = EPSILON;
                return vec3(0.0f);
            }
        }
        case MATERIAL_TYPE::PHONG: {
            float diffuse_percent = 0.5;
            float p = get_random_float();
            if (p <= diffuse_percent) {
                // diffuse
                float x1 = get_random_float();
                float x2 = get_random_float();
                float z = std::abs(1.0f - 2.0f * x1);
                float r = std::sqrt(1.0f - z * z);
                float phi = x2 * 2.0f * PI;
                float x = r * std::cos(phi);
                float y = r * std::sin(phi);
                vec3 local_wi = {x, y, z};
                vec3 world_wi = toWorld(local_wi, N);
                // set wi
                wi = world_wi;
                // set pdf
                if (glm::dot(wi, N) > 0.0f) {
                    pdf = diffuse_percent * 0.5f / PI;
                    return diffuse_percent * my_Kd / PI;
                } else {
                    pdf = EPSILON;
                    return vec3(0.0f);
                }

            } else {
                // phong
                float x1 = get_random_float();
                float x2 = get_random_float();
                float theta = std::acos(std::pow(x1, 1.0f / (shine_exponent + 1.0f)));
                float t = std::pow(x1, 2.0f / (shine_exponent + 1.0f));
                float z = std::sqrt(t);
                float r = std::sqrt(1.0f - z * z);
                float phi = x2 * 2.0f * PI;
                float x = r * std::cos(phi);
                float y = r * std::sin(phi);
                vec3 reflect_dir = reflect(N, wo);
                vec3 local_wi = {x, y, z};
                vec3 world_wi = toWorld(local_wi, reflect_dir);
                wi = world_wi;
                if (glm::dot(wi, N) > 0.0f) {
                    float cos_phi = glm::dot(wi, reflect_dir);
                    vec3 specular = my_Ks * std::pow(cos_phi, shine_exponent) * (shine_exponent + 2.0f) / (2.0f * PI);
                    pdf = (1.0f - diffuse_percent) * std::pow(cos_phi, shine_exponent) * (shine_exponent + 2.0f) / (2.0f * PI);
                    return (1.0f - diffuse_percent) * specular;
                } else {
                    pdf = EPSILON;
                    return {0.0f, 0.0f, 0.0f};
                }

            }
        }
        case MATERIAL_TYPE::MICRO_FACET: {
            float x1 = get_random_float();
            float x2 = get_random_float();
            float a = roughness * roughness;
            float a2 = a * a;
            float theta = std::acos(std::sqrt((1.0f - x1) / (x1 * (a2 - 1.0f) + 1.0f)));
            float phi = x2 * 2.0f * PI;

            float z = std::cos(theta);
            float r = std::sqrt(1.0f - z * z);
            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            vec3 local_h = {x, y, z};
            vec3 world_h = toWorld(local_h, N);
            // set wi
            wi = reflect(world_h, wo);

            if (glm::dot(wi, N) > 0.0f) {
                float cos1 = std::max(glm::dot(N, wi), 0.0f);
                vec3 h = glm::normalize(wo + wi);
                float cos_theta = std::max(glm::dot(h, N), EPSILON);
                float exp = (a2 - 1.0f) * cos_theta * cos_theta + 1.0f;
                float D = a2 / (PI * exp * exp);
                // set pdf
                pdf = D * cos_theta / (4.0f * std::max(glm::dot(wo, h), 0.0f) + EPSILON);

                float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
                float G = G_schlick_ggx(N, wo, k) * G_schlick_ggx(N, wi, k);

                vec3 F0(0.04);
                F0 = F0 * (1 - metallic) + metallic * my_Kd;
                vec3 F = F0 + std::pow(1.0f - cos1, 5.0f) * (vec3(1.0f) - F0);
                vec3 f_cook_torrance =
                        D * F * G / (4 * std::max(glm::dot(wo, N), 0.0f) * std::max(glm::dot(wi, N), 0.0f) + EPSILON);
                vec3 _kd = (vec3(1.0f) - F) * (1 - metallic);
                return _kd * my_Kd / PI + my_Ks * f_cook_torrance;
            } else {
                // set pdf
                pdf = EPSILON;
                return vec3(0.0f);
            }
        }
        case MATERIAL_TYPE::MIRROR: {
            wi = reflect(N, wo);
            pdf = 1.0f;
            return my_Ks / std::max(glm::dot(wi, N), EPSILON);
        }
        case MATERIAL_TYPE::GLASS: {
            float f = fresnel(N, wo, ior);
            float p = get_random_float();
            if (p <= f) {
                // reflect
                wi = reflect(N, wo);
                pdf = f;
                return my_Ks * f / std::max(glm::dot(wi, N), EPSILON);
            } else {
                // refract
                wi = refract(N, wo, ior);
                pdf = 1.0f - f;
                return my_Ks * (1.0f - f) / std::max(std::abs(glm::dot(wi, N)), EPSILON);
            }

        }

    }
}
