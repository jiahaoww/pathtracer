//
// Created by jiahaoww on 2022/2/24.
//

#include "scene.h"
#include <thread>
#include <future>

Scene::Scene(int w, int h): width(w), height(h) {
    frame_buffer.resize(w * h);
}

Intersection Scene::intersect(const Ray &ray) const {
//    std::cout << "scene bvh" << std::endl;
//    bvh->print_aabb(bvh->root);
    return bvh->intersect(ray);
}

vec3 Scene::castRay(const Ray &ray) {
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
    if (inter.m->has_emission) {
        return inter.m->emit;
    }
    // return vec3(0.0f);
    vec3 L_dir(0.0f);
    vec3 L_in_dir(0.0f);
    Intersection light_inter;
    float light_pdf;
    sample_light(light_inter, light_pdf);
    vec3 light_dir = glm::normalize(light_inter.pos - inter.pos);
    Ray light_ray(inter.pos + light_dir * 0.01f, light_dir);
    Intersection i = intersect(light_ray);
    // not blocked
    if (glm::length(i.pos - light_inter.pos) < 0.01) {
        float d = glm::length(light_inter.pos - inter.pos);
        L_dir = inter.m->eval(inter.normal, -ray.dir, light_dir) * light_inter.m->emit * glm::dot(light_inter.normal, -light_dir)
                * glm::dot(inter.normal, light_dir) / (d * d * light_pdf);
    }
    if (get_random_float() < rr) {
        vec3 wi = inter.m->sample(inter.normal, -ray.dir);
        wi = glm::normalize(wi);
        float pdf = inter.m->pdf(inter.normal, -ray.dir, wi);
        Ray obj_ray(inter.pos + 0.01f * wi, wi);
        Intersection i = intersect(obj_ray);
        if (i.has && !intersect(obj_ray).m->has_emission) {
            L_in_dir = castRay(obj_ray) * inter.m->eval(inter.normal, -ray.dir, wi) * glm::dot(inter.normal, wi) / (pdf * rr);
        }
    }
    vec3 color = L_dir + L_in_dir;
    vec3 hit_color = {clamp(0.0f, 1.0f, color.x), clamp(0.0f, 1.0f, color.y), clamp(0.0f, 1.0f, color.z)};
    return hit_color;
}

void Scene::build_bvh() {
    std::cout << "start building scene BVH" << std::endl;
    bvh = new BVH(objects);
    std::cout << bvh->root->size << std::endl;
    bvh->print_aabb(bvh->root);
}

void Scene::add_obj(Object *obj) {
    objects.push_back(obj);
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
            }
        }
    }
}

void Scene::render() {
    float scale = tan(fov * 0.5 * PI / 180.0);
    float imageAspectRatio = width / (float) height;
    vec3 eye_pos(278, 273, -800);


    // change the spp value to change sample ammount
    int spp = 256;
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < height; ++j) {
        int threads = std::thread::hardware_concurrency();
        int offset = width / threads;
        std::vector<std::future<void>> futures;

        for (uint32_t i = 0; i < threads; ++i) {
            int begin_offset = offset * i;
            int end_offset = i + 1 == threads ? width : begin_offset + offset;

            std::future<void> task = std::async(std::launch::async, [&](int begin_offset, int end_offset) {
                for (int i = begin_offset; i < end_offset; ++i) {
                    // generate primary ray direction
                    for (int k = 0; k < spp; k++) {
                        float x = (2 * (i + get_random_float()) / (float)width - 1) *
                                  imageAspectRatio * scale;
                        float y = (1 - 2 * (j + get_random_float()) / (float)height) * scale;

                        vec3 dir = normalize(vec3(-x, y, 1));
                        frame_buffer[j * width + i] += castRay(Ray(eye_pos, dir)) / (float)spp;
                    }
                }
            }, begin_offset, end_offset);
            futures.emplace_back(std::move(task));
        }

        for (auto &&future: futures) {
            future.get();
        }
        UpdateProgress(j / (float) height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE *fp = fopen("binary.ppm", "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (auto i = 0; i < height * width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].x), 0.6f));
        color[1] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].y), 0.6f));
        color[2] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}
