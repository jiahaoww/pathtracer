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

class Loader {
public:
    static std::vector<std::vector<Object*>> load(std::string filepath, Material *m) {
        objl::Loader loader;
        std::cout << filepath << std::endl;
        loader.LoadFile(std::move(filepath));

        std::cout << "loaded meshes " << loader.LoadedMeshes.size() << " " << loader.LoadedMaterials.size() << std::endl;

        std::vector<std::vector<Object*>> meshes;

        for(auto& mesh: loader.LoadedMeshes) {
            std::vector<Object *> triangle_list;
            auto material = mesh.MeshMaterial;

            vec3 emit(0.0f);
            if (mesh.MeshMaterial.name == "Light") {
                emit = vec3(17, 12, 4);
            }
            if (mesh.MeshMaterial.name == "Light1") {
                // std::cout << "light1" << std::endl;
                emit = vec3(901.803, 901.803, 901.803);
            }
            if (mesh.MeshMaterial.name == "Light2") {
                emit = vec3(100.0, 100.0, 100.0);
            }
            if (mesh.MeshMaterial.name == "Light3") {
                emit = vec3(11.1111, 11.1111, 11.1111);
            }
            if (mesh.MeshMaterial.name == "Light4") {
                emit = vec3(1.23457, 1.23457, 1.23457);
            }
            if (mesh.MeshMaterial.name == "Light5") {
               emit = vec3(800.0, 800.0, 800.0);
            }

            auto materialType = DIFFUSE;
//            if (mesh.MeshMaterial.name == "Plane1" || mesh.MeshMaterial.name == "Plane2" || mesh.MeshMaterial.name == "Plane3" || mesh.MeshMaterial.name == "Plane4") {
//                materialType = MICRO_FACET;
//            }

            float ior = material.Ni;
            float shine_exponent = material.Ns;
            shine_exponent = 16.0f;
            vec3 Kd = {material.Kd.X, material.Kd.Y, material.Kd.Z};
            vec3 Ks = {material.Ks.X, material.Ks.Y, material.Ks.Z};
            Material* mym = new Material(materialType, Kd, Ks, emit, ior, shine_exponent, material.map_Kd, material.map_Ks);
//            mym->roughness = 0.1;
//            mym->metallic = 1.0;
            Material* tm = m == nullptr ? mym : m;
            if(mesh.MeshMaterial.map_Kd.size() > 0) {
                // read texture
                std::string texture_path = "../models/veach-mis" + mesh.MeshMaterial.map_Kd;
                Texture* texture = new Texture(texture_path.c_str());
                tm->texture = texture;
            }
            for(int i = 0; i < mesh.Vertices.size(); i+=3) {
                vec3 vertices[3];
                vec3 normals[3];
                vec2 texture_coords[3];
                for (int j = 0; j < 3; j++) {
                    vertices[j] = {mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z};
                    normals[j] =  {mesh.Vertices[i + j].Normal.X,mesh.Vertices[i + j].Normal.Y,mesh.Vertices[i + j].Normal.Z};
                    texture_coords[j] = {mesh.Vertices[i + j].TextureCoordinate.X - (int)mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y - (int)mesh.Vertices[i + j].TextureCoordinate.Y};
                }
                Object* triangle = new Triangle(vertices[0], vertices[1], vertices[2], normals[0], normals[1], normals[2], texture_coords[0], texture_coords[1], texture_coords[2], tm);
                triangle_list.push_back(triangle);
            }
            meshes.push_back(triangle_list);
        }
        return meshes;
    }
};


#endif //PATHTRACER_LOADER_H
