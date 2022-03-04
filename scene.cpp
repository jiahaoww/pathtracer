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
    float light_area = 0.0f;
    for (auto& obj: objects) {
        if (obj->m->has_emission) {
            light_area += obj->get_area();
        }
    }
    float p = get_random_float();
    float area = p * light_area;
    float current_light_area = 0.0f;
    for (auto& obj: objects) {
        if (obj->m->has_emission) {
            current_light_area += obj->get_area();
            if (current_light_area >= area) {
                obj->sample(inter, pdf);
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

vec3 Scene::castRay(const Ray &ray) {
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
    if (inter.m->has_emission) {
        return inter.m->emit;
    }
    bool is_mirror = inter.m->type != DIFFUSE;
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


    vec3 wi = inter.m->sample(inter.normal, -ray.dir);
    float pdf = inter.m->pdf(inter.normal, -ray.dir, wi);
    Ray obj_ray(inter.pos, wi);
    Intersection obj_ray_inter = intersect(obj_ray);

    if (!obj_ray_inter.has || (!is_mirror && obj_ray_inter.m->has_emission)) {
        return glm::clamp(L_dir, 0.0f, 1.0f);
    }


    L_in_dir = castRay(obj_ray)
               * inter.m->eval(inter.normal, -ray.dir, wi, kd)
               * std::abs(glm::dot(inter.normal, wi))
               / (pdf * rr);

    vec3 color = L_dir + L_in_dir;
    return glm::clamp(color, 0.0f, 1.0f);
}

vec3 Scene::castRay_merge(const Ray &ray) {
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
    if (inter.m->has_emission) {
        return inter.m->emit;
    }
    bool is_mirror = inter.m->type != DIFFUSE;
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






