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

        std::vector<glm::vec3> vertices;

        for (objl::Vertex vertex: mesh.Vertices) {
            vertices.emplace_back(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
        }

        int current = 0;
        glm::uvec3 face;
        for (uint32_t index: mesh.Indices) {
            face[current++] = index;
            if (current == 3) {
                objects.emplace_back(new Triangle(vertices[face[0]], vertices[face[1]], vertices[face[2]], m));
                current = 0;
            }
        }
        std::cout << "face number: " << objects.size() << std::endl;
        return objects;
    }
};


#endif //PATHTRACER_LOADER_H
