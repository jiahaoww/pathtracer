//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_LOADER_H
#define PATHTRACER_LOADER_H

#include <string>
#include <utility>
#include "global.h"
#include "triangle.h"
#include "OBJ_Loader.h"
#include "Texture.h"
#include <map>

#include "EmbreeAccelerator.h"

class Loader {
public:


    static std::tuple<EmbreeAccelerator, std::vector<std::vector<Object *>>>
    load(std::string &base_path, std::string &obj_name, Material *m, std::map<std::string, vec3> &lights_emit) {
        EmbreeAccelerator accelerator;
        objl::Loader loader;
        loader.LoadFile(base_path + obj_name);
        std::vector<std::vector<Object *>> meshes;

        for (auto &mesh: loader.LoadedMeshes) {
            std::vector<Object *> triangle_list;
            auto material = mesh.MeshMaterial;
            vec3 emit(0.0f);
            for (auto iter = lights_emit.begin(); iter != lights_emit.end(); iter++) {
                std::string mtl_name = iter->first;
                vec3 light_emit = iter->second;
                if (mesh.MeshMaterial.name == mtl_name) {
                    emit = light_emit;
                    break;
                }
            }

            auto materialType = PHONG;


            float ior = material.Ni;
            float shine_exponent = material.Ns;
            vec3 Kd = {material.Kd.X, material.Kd.Y, material.Kd.Z};
            vec3 Ks = {material.Ks.X, material.Ks.Y, material.Ks.Z};

//            if (shine_exponent > 100.0f) {
//                materialType = MIRROR;
//                Ks = {1.0f, 1.0f, 1.0f};
//                // Kd = {0.8f, 0.8f, 0.8f};
//            }

            if (ior > 1.0f) {
                Ks = {1.0f, 1.0f, 1.0f};
                materialType = GLASS;
            }

            Material *mym = new Material(materialType, Kd, Ks, emit, ior, shine_exponent, material.map_Kd,
                                         material.map_Ks);
            Material *tm = m == nullptr ? mym : m;
            tm->roughness = 0.4;
            tm->metallic = 0.0;

            if (mesh.MeshMaterial.map_Kd.size() > 0) {
                // read texture
                std::string texture_path = base_path + mesh.MeshMaterial.map_Kd;
                Texture *texture = new Texture(texture_path.c_str());
                tm->texture = texture;
            }
            for (int i = 0; i < mesh.Vertices.size(); i += 3) {
                vec3 vertices[3];
                vec3 normals[3];
                vec2 texture_coords[3];
                for (int j = 0; j < 3; j++) {
                    vertices[j] = {mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y,
                                   mesh.Vertices[i + j].Position.Z};
                    normals[j] = {mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y,
                                  mesh.Vertices[i + j].Normal.Z};
                    texture_coords[j] = {mesh.Vertices[i + j].TextureCoordinate.X,
                                         mesh.Vertices[i + j].TextureCoordinate.Y};
                }
                Triangle *triangle = new Triangle(vertices[0], vertices[1], vertices[2], normals[0], normals[1],
                                                  normals[2], texture_coords[0], texture_coords[1], texture_coords[2],
                                                  tm);

                std::vector<Triangle *> one_face = {triangle};
                accelerator.add_faces(one_face);
                triangle_list.push_back(triangle);
            }
            meshes.push_back(triangle_list);
        }
        return {accelerator, meshes};
    }

    static std::vector<Object *> load_single_mesh(std::string filepath, Material *m) {
        objl::Loader loader;
        loader.LoadFile(std::move(filepath));
        std::vector<Object *> objects;
        auto mesh = loader.LoadedMeshes[0];
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            vec3 vertices[3];

            for (int j = 0; j < 3; j++) {
                vertices[j] = {mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y,
                               mesh.Vertices[i + j].Position.Z};
            }
            Object *triangle = new Triangle(vertices[0], vertices[1], vertices[2], m);
            objects.push_back(triangle);
        }
        return objects;
    }
};


#endif //PATHTRACER_LOADER_H
