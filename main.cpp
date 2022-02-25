#include <iostream>
#include <chrono>
#include "loader.h"
#include "mesh.h"
#include "scene.h"

int main() {
    // std::cout << "Hello, World!" << std::endl;
    const float a = 0.1;
    Material* red = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.63f, 0.065f, 0.05f), vec3(1.0f), vec3(0.0f), 1.85);
    red->roughness =a;

    Material* green = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.14f, 0.45f, 0.091f), vec3(1.0f), vec3(0.0f), 1.85);
    green->roughness = a;

    Material* white = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.725f, 0.71f, 0.68f), vec3(1.0f), vec3(0.0f), 1.85);
    white->roughness =a;

    Material* floor_m = new Material(MATERIAL_TYPE::MICRO_FACET, vec3(0.725f, 0.71f, 0.68f), vec3(1.0f), vec3(0.0f), 1.85);
    floor_m->roughness = a;
    floor_m->metallic = 0.0f;

    Material* light = new Material(MATERIAL_TYPE::DIFFUSE, vec3(1.0f), vec3(1.0f), 8.0f * vec3(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * vec3(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f * vec3(0.737f+0.642f,0.737f+0.159f,0.737f), 1.85);

    Mesh* floor = new Mesh(Loader::load("models/cornellbox/floor.obj", floor_m));
    Mesh* left = new Mesh(Loader::load("models/cornellbox/left.obj", red));
    Mesh* right = new Mesh(Loader::load("models/cornellbox/right.obj", green));
    Mesh* short_box = new Mesh(Loader::load("models/cornellbox/shortbox.obj", white));
    Mesh* tall_box = new Mesh(Loader::load("models/cornellbox/tallbox.obj", white));
    Mesh* light_source = new Mesh(Loader::load("models/cornellbox/light.obj", light));
    Scene scene(1024, 1024);

    scene.add_obj(floor);
    scene.add_obj(left);
    scene.add_obj(right);
    scene.add_obj(short_box);
    scene.add_obj(tall_box);
    scene.add_obj(light_source);
    scene.build_bvh();

    auto start = std::chrono::system_clock::now();
    scene.render();
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}
