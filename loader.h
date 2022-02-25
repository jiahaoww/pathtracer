//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_LOADER_H
#define PATHTRACER_LOADER_H

#include <string>
#include <utility>
#include "global.h"
#include "triangle.h"
#include "OBJ_Loader.hpp"

class Loader {
public:
    static std::vector<Object *> load(std::string filepath, Material *m) {
        objl::Loader loader;
        loader.LoadFile(std::move(filepath));
        std::vector<Object *> objects;
        if (loader.LoadedMeshes.size() != 1) {
            return objects;
        }
        const auto &mesh = loader.LoadedMeshes[0];

        for(int i = 0; i < mesh.Vertices.size(); i+=3) {
            vec3 v[3];
            for (int j = 0; j < 3; j++) {
                v[j] = {mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z};
            }
            Object* obj = new Triangle(v[0], v[1], v[2], m);
            objects.push_back(obj);
        }

        return objects;
    }
};


#endif //PATHTRACER_LOADER_H
