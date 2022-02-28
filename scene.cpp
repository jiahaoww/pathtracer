//
// Created by jiahaoww on 2022/2/24.
//

#include "scene.h"
#include "triangle.h"
#include "stb/stb_image_write.h"
#include <thread>
#include <future>

Scene::Scene(int w, int h): width(w), height(h) {
    frame_buffer.resize(w * h);
    image.resize(w * h * 3);
}

Scene::Scene(int w, int h, Camera &cam): width(w), height(h), camera(cam) {
    frame_buffer.resize(w * h);
    image.resize(w * h * 3);
}


Scene::Scene(Camera &cam) {
    width = cam.width;
    height = cam.height;
    camera = cam;
    frame_buffer.resize(width * height);
    image.resize(width * height * 3);
}

Intersection Scene::intersect(const Ray &ray) const {
    return bvh->intersect(ray);
}

vec3 Scene::castRay(const Ray &ray) {
    Intersection inter = intersect(ray);
    if (!inter.has) {
        return vec3(0.0f);
    }
//    if (inter.m->has_emission) {
//        return inter.m->emit;
//    }
    // return vec3(0.0f);
    vec3 L_dir(0.0f);
    vec3 L_in_dir(0.0f);
    Intersection light_inter;
    float light_pdf=0.0f;
    sample_light(light_inter, light_pdf);
    vec3 light_dir = glm::normalize(light_inter.pos - inter.pos);
    Ray light_ray(inter.pos, light_dir);
    Intersection i = intersect(light_ray);
    // not blocked

    vec3 kd = inter.m->Kd;
    if (inter.m->texture != nullptr) {
        vec2 tex_coord;
        Object* object = inter.obj;
        Triangle* triangle = dynamic_cast<Triangle *>(object);
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
        kd = {color.x / 255.0f, color.y / 255.0f, color.z / 255.0};
    }
    if (i.has && i.obj == light_inter.obj) {
        float d = glm::length(light_inter.pos - inter.pos);
        L_dir = inter.m->eval(inter.normal, -ray.dir, light_dir, kd) * light_inter.m->emit * std::max(glm::dot(light_inter.normal, -light_dir), 0.0f)
                 / (d * d * light_pdf);
    }
    vec3 dir_color = L_dir + inter.m->emit;
    return dir_color;
    return {clamp(0.0f, 1.0f, dir_color.x), clamp(0.0f, 1.0f, dir_color.y), clamp(0.0f, 1.0f, dir_color.z)};
    return L_dir + inter.m->emit;

    if (get_random_float() < rr) {
        vec3 wi = inter.m->sample(inter.normal, -ray.dir);
        wi = glm::normalize(wi);
        float pdf = inter.m->pdf(inter.normal, -ray.dir, wi);
        Ray obj_ray(inter.pos, wi);
        Intersection i = intersect(obj_ray);
        if (i.has && !intersect(obj_ray).m->has_emission) {
            L_in_dir = castRay(obj_ray) * inter.m->eval(inter.normal, -ray.dir, wi, kd) / (pdf * rr);
        }
    }
    vec3 color = L_dir + L_in_dir + inter.m->emit;
    vec3 hit_color = {clamp(0.0f, 1.0f, color.x), clamp(0.0f, 1.0f, color.y), clamp(0.0f, 1.0f, color.z)};
    return color;
}

void Scene::build_bvh() {
    std::cout << "start building scene BVH" << std::endl;
    bvh = new BVH(objects);
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
                break;
            }
        }
    }
}

void Scene::render(int spp) {
    float scale = tan(camera.fov * 0.5 * PI / 180.0);
    float imageAspectRatio = width / (float) height;
    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 inverse_view = glm::inverse(view);



    // change the spp value to change sample amount
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < height; ++j) {
        int threads = std::thread::hardware_concurrency() << 1;
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

                        vec3 dir = glm::normalize(inverse_view * vec4(vec3(x, y, -1.0f), 0.0f));
                        frame_buffer[j * width + i] += castRay(Ray(camera.eye, dir)) / (float)spp;
                    }
                }
            }, begin_offset, end_offset);
            futures.emplace_back(std::move(task));
        }
        UpdateProgress(j / (float) height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
//    FILE *fp = fopen("binary.png", "wb");
//    (void) fprintf(fp, "P6\n%d %d\n255\n", width, height);
    const float gamma = 0.6;
    for (auto i = 0; i < height * width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].x), gamma));
        color[1] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].y), gamma));
        color[2] = (unsigned char) (255 * std::pow(clamp(0, 1, frame_buffer[i].z), gamma));
        image[3 * i] = color[0];
        image[3 * i + 1] = color[1];
        image[3 * i + 2] = color[2];
        //fwrite(color, 1, 3, fp);
    }
    //fclose(fp);
    stbi_write_png("result.png", width, height, 3, image.data(), 0);
}



