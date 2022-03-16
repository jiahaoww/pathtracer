//
// Created by jiahaoww on 2022/2/24.
//

#include "scene.h"
#include "triangle.h"
#include "stb/stb_image_write.h"


Scene::Scene(Camera &cam) {
    width = cam.width;
    height = cam.height;
    camera = cam;
    acc = nullptr;
}

Intersection Scene::intersect(const Ray &ray) const {
    if (acc != nullptr) {
        return acc->hit(ray);
    }
    return bvh->intersect(ray);
}

void Scene::add_obj(Object *obj) {
    objects.push_back(obj);
}

void Scene::build_bvh() {
    std::cout << "start building scene BVH" << std::endl;
    bvh = new BVH(objects);
}

void Scene::sample_light(Intersection &inter, float &pdf) {
    int light_cnt = 0;
    float light_area = 0.0f;
    for (auto& obj: objects) {
        if (obj->m->has_emission) {
            light_area += obj->get_area();
            light_cnt++;
        }
    }
    float p = get_random_float();
    float idx = p * light_cnt;
    float area = p * light_area;
    float current_light_area = 0.0f;
    int cnt = 0;
    for (auto& obj: objects) {
        if (obj->m->has_emission) {
            cnt++;
            current_light_area += obj->get_area();
//            if (current_light_area >= area) {
//                obj->sample(inter, pdf);
//                pdf *= obj->get_area() / light_area;
//                break;
//            }
            if (cnt >= idx) {
                obj->sample(inter, pdf);
                // pdf /= light_cnt;
                // std::cout << pdf << std::endl;
                // pdf *= obj->get_area() / light_area;
                break;
            }
        }
    }
}

vec3 get_intersection_color(Intersection& inter, bool& has_texture) {
    if (inter.m->texture == nullptr) {
        has_texture = false;
        return inter.m->Kd;
    }
    has_texture = true;
    vec2 tex_coord;
    Object* object = inter.obj;
    auto triangle = dynamic_cast<Triangle *>(object);
    if (triangle == nullptr) {
        throw std::bad_cast();
    } else {
        tex_coord = (float)(inter.b1) * triangle->tex[1] + (float)(inter.b2) * triangle->tex[2] + (float)(1.0f - inter.b1 - inter.b2) * triangle->tex[0];
    }
    int w = inter.m->texture->width;
    int h = inter.m->texture->height;
    int p = h * tex_coord.y;
    int q = w * tex_coord.x;
    p = (p % h + h ) % h;
    p = h - 1 - p;
    q = (q % w + w) % w;
    glm::u8vec3 color = (*inter.m->texture)[p][q];
    return {color.x / 255.0f, color.y / 255.0f, color.z / 255.0};
}

vec3 Scene::calculate_direct_light(const Ray& ray, const Intersection& inter, const vec3& kd, float& pdf) {
    vec3 wo = -ray.dir;
    vec3 L_dir(0.0f);
    // sample light
    Intersection light_inter;
    float light_area_pdf;
    sample_light(light_inter, light_area_pdf);
    vec3 light_dir = glm::normalize(light_inter.pos - inter.pos);
    Ray light_ray(inter.pos, light_dir);
    Intersection light_ray_inter = intersect(light_ray);

    bool light_hit = light_ray_inter.has && light_ray_inter.m->has_emission && glm::dot(light_ray_inter.normal, light_dir) < 0.0f;

    if (!light_hit) {
        return L_dir;
    }

    float d = glm::length(light_ray_inter.pos - inter.pos);
    pdf = light_area_pdf * d * d / glm::dot(-light_dir, light_ray_inter.normal);
    vec3 brdf = inter.m->eval(inter.normal, wo, light_dir, kd);
    L_dir = brdf
            * light_ray_inter.m->emit
            * std::max(glm::dot(inter.normal, light_dir), 0.0f)
            / pdf;
    if (glm::length(brdf) < EPSILON) {
        pdf = 0.0f;
    }
    if (isnan(L_dir.x) || isnan(L_dir.y) || isnan(L_dir.z)) {
        L_dir = {0.0f, 0.0f, 0.0f};
    }
    return L_dir;
}

vec3 Scene::castRay(const Ray &ray, int depth) {
    const float max_radiance = 10.0f;
    vec3 wo = -ray.dir;
    if (depth > max_depth) {
        return vec3(0.0f);
    }
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
    if (inter.m->has_emission && depth == 0) {
        return glm::fclamp(inter.m->emit, 0.0f, 1.0f);
    }
    bool has_texture = false;
    vec3 kd = get_intersection_color(inter, has_texture);

    float light_pdf = 0.0f;
    vec3 L_dir_light = calculate_direct_light(ray, inter, kd, light_pdf);
//    return L_dir_light;

    // sample brdf
    vec3 wi = inter.m->sample(inter.normal, wo);
    float pdf = inter.m->pdf(inter.normal, wo, wi);
    Ray obj_ray(inter.pos, wi);
    Intersection obj_ray_inter = intersect(obj_ray);

    // not hit
    if (!obj_ray_inter.has) {
        return glm::fclamp(L_dir_light, 0.0f, max_radiance);
    }

    bool hit_light = obj_ray_inter.m->has_emission && glm::dot(wi, obj_ray_inter.normal) < 0.01f;

    float brdf_pdf = 0.0f;
    vec3 L_dir_brdf(0.0f);

    // if wi hits light, no indirect light,
    vec3 color(0.0f);
    if (hit_light) {
        brdf_pdf = pdf;
        L_dir_brdf = obj_ray_inter.m->emit // (17, 12, 4)
                 * inter.m->eval(inter.normal, wo, wi, kd) // kd / pi or 0
                 * glm::dot(inter.normal, wi)
                 / brdf_pdf; // 0.5 / pi
        if (L_dir_brdf.x > 20.0f) {
            // exit(-1);
        }
        float light_pdf2 = std::pow(light_pdf, 2.0f);
        float brdf_pdf2 = std::pow(brdf_pdf, 2.0f);
        float denominator = light_pdf2 + brdf_pdf2;
        vec3 L_dir = L_dir_light * light_pdf2 / denominator + L_dir_brdf * brdf_pdf2 / denominator;
        color = L_dir;
        if (!using_mis) {
            color = L_dir_light;
        }
    }
    // if wi hits an object, indirect light
    else {
        float p = get_random_float();
        if (p > rr) {
            return glm::fclamp(L_dir_light, 0.0f, max_radiance);
        }
        vec3 L_in_dir = castRay(obj_ray, depth + 1)
                   * inter.m->eval(inter.normal, wo, wi, kd)
                   * std::abs(glm::dot(inter.normal, wi))
                   / (pdf * rr);
        color = L_dir_light + L_in_dir;
    }
    if (isnan(color.x) || isnan(color.y) || isnan(color.z)) {
        color = {0.0f, 0.0f, 0.0f};
    }
    return glm::fclamp(color, 0.0f, max_radiance);
}

vec3 Scene::castRay_merge(const Ray &ray) {
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
    if (inter.m->has_emission) {
        return inter.m->emit;
    }
    bool is_mirror = false;
    vec3 L_dir(0.0f), L_in_dir(0.0f);
    Intersection light_inter;
    float light_pdf;
    sample_light(light_inter, light_pdf);
    vec3 light_dir = glm::normalize(light_inter.pos - inter.pos);
    Ray light_ray(inter.pos, light_dir);
    Intersection light_ray_inter = intersect(light_ray);

    bool has_texture = false;
    vec3 kd = get_intersection_color(inter, has_texture);

    if (light_ray_inter.has && light_ray_inter.obj == light_inter.obj) {
        float d = glm::length(light_inter.pos - inter.pos);
        L_dir = inter.m->eval(inter.normal, -ray.dir, light_dir, kd)
                * light_inter.m->emit
                * glm::dot(inter.normal, light_dir)
                * glm::dot(light_inter.normal, -light_dir)
                / (d * d * light_pdf);
    }

    float p = get_random_float();
    if (p > rr) {
        return glm::clamp(L_dir, 0.0f, 1.0f);
    }

    vec3 wi;
    float pdf;
    vec3 bsdf = inter.m->sample_f(inter.normal, -ray.dir, wi, pdf, kd);

    Ray obj_ray(inter.pos, wi);
    Intersection obj_ray_inter = intersect(obj_ray);

    if (!obj_ray_inter.has || (!is_mirror && obj_ray_inter.m->has_emission)) {
        return glm::clamp(L_dir, 0.0f, 1.0f);
    }


    L_in_dir = castRay_merge(obj_ray)
               * bsdf
               * std::abs(glm::dot(inter.normal, wi))
               / (pdf * rr);

    vec3 color = L_dir + L_in_dir;
    return glm::clamp(color, 0.0f, 1.0f);
}






