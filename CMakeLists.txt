cmake_minimum_required(VERSION 3.17)
project(pathTracer)

set(CMAKE_CXX_STANDARD 17)

add_executable(pathTracer main.cpp global.h material.cpp material.h object.h intersection.h ray.h triangle.cpp triangle.h aabb.h bvh.h mesh.cpp mesh.h loader.h scene.cpp scene.h Texture.h Texture.cpp)

target_include_directories(pathTracer PRIVATE include)