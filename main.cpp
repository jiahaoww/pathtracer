#include <iostream>
#include <chrono>
#include <map>
#include "loader.h"
#include "mesh.h"
#include "scene.h"
#include "xml.h"
#include "sphere.h"


int main() {
//    int model = 1;
//    int spp = 16;
//    std::string name;
//    switch (model) {
//        case 1: {
//            name = "cornell-box";
//            break;
//        }
//        case 2: {
//            name = "veach-mis";
//            break;
//        }
//        case 3: {
//            name = "bedroom";
//            break;
//        }
//        default:
//            name = "cornell-box";
//    }
//    std::string base_path = "../models/" + name + "/";
//    std::string model_name = name + ".obj";
//    std::string xml_name = name + ".xml";
//    std::string xml_path = base_path + xml_name;
//    Camera camera{};
//    std::map <std::string, vec3> light_emits = XML::load_xml(xml_path, camera);
//    auto meshes = Loader::load(base_path, model_name, nullptr, light_emits);
//
//    std::vector<Mesh *> MeshList;
//    for (auto &mesh: meshes) {
//        MeshList.push_back(new Mesh(mesh));
//    }
//
//    Scene scene(camera);
//    for (auto Mesh: MeshList) {
//        scene.add_obj(Mesh);
//    }
//    scene.build_bvh();
//    auto start = std::chrono::system_clock::now();
//    scene.render(spp);
//    auto stop = std::chrono::system_clock::now();
//
//    std::cout << "Render complete: \n";
//    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
//    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count()
//              << " minutes\n";
//    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
//              << " seconds\n";
//    return 0;

    const float a = 0.1;
    Material* red = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.63f, 0.065f, 0.05f), vec3(1.0f), vec3(0.0f), 1.85);
    red->roughness =a;

    Material* green = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.14f, 0.45f, 0.091f), vec3(1.0f), vec3(0.0f), 1.85);
    green->roughness = a;

    Material* white = new Material(MATERIAL_TYPE::DIFFUSE, vec3(0.725f, 0.71f, 0.68f), vec3(1.0f), vec3(0.0f), 1.85);
    white->roughness =a;

    Material* metal = new Material(MATERIAL_TYPE::MICRO_FACET, vec3(0.725f, 0.71f, 0.68f), vec3(186 / 255.0f, 85 / 255.0f, 211 / 255.0f ), vec3(0.0f), 1.00);
    metal->roughness = 0.03;
    metal->metallic = 1.0f;

    Material* pure_mirror = new Material(MATERIAL_TYPE::MIRROR, vec3(0.0f), vec3(1.0f), vec3(0.0f), 1.0f);
    Material* pure_glass = new Material(MATERIAL_TYPE::GLASS, vec3(0.0f), vec3(1.0f), vec3(0.0f), 1.5f);

    Material* light = new Material(MATERIAL_TYPE::DIFFUSE, vec3(1.0f), vec3(1.0f), 8.0f * vec3(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * vec3(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f * vec3(0.737f+0.642f,0.737f+0.159f,0.737f), 1.85);

    Mesh* floor = new Mesh(Loader::load_single_mesh("../models/cornellbox/floor.obj", white));
    Mesh* left = new Mesh(Loader::load_single_mesh("../models/cornellbox/left.obj", red));
    Mesh* right = new Mesh(Loader::load_single_mesh("../models/cornellbox/right.obj", green));
    Mesh* short_box = new Mesh(Loader::load_single_mesh("../models/cornellbox/shortbox.obj", white));
    Mesh* tall_box = new Mesh(Loader::load_single_mesh("../models/cornellbox/tallbox.obj", white));
    Mesh* light_source = new Mesh(Loader::load_single_mesh("../models/cornellbox/light.obj", light));

    float x = 300;
    float offset = 150;

    Sphere mirror_sphere(vec3(x + offset, 250, 100), 100);
    mirror_sphere.m = pure_mirror;

    Sphere glass_sphere(vec3(x - offset, 250, 100), 100);
    glass_sphere.m = pure_glass;

    Sphere metal_sphere(vec3(x, 250, 400), 100);
    metal_sphere.m = metal;

    Camera cam(vec3(278, 273, -800), vec3(278, 273, -799), vec3(0, 1, 0), 40, 1024, 1024);
    Scene scene(cam);



    scene.add_obj(floor);
    scene.add_obj(left);
    scene.add_obj(right);
    // scene.add_obj(tall_box);
    scene.add_obj(light_source);
    scene.add_obj(&glass_sphere);
    scene.add_obj(&mirror_sphere);
    scene.add_obj(&metal_sphere);
    scene.build_bvh();

    auto start = std::chrono::system_clock::now();
    scene.render(1024);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}
