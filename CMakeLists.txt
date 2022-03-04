cmake_minimum_required(VERSION 3.17)
project(pathTracer)

set(CMAKE_CXX_STANDARD 17)

add_executable(pathTracer main.cpp include/global.h material.cpp include/material.h include/object.h include/intersection.h include/ray.h triangle.cpp include/triangle.h include/aabb.h include/bvh.h mesh.cpp include/mesh.h include/loader.h scene.cpp include/scene.h include/Texture.h Texture.cpp include/camera.h include/xml.h sphere.cpp include/sphere.h include/renderer.h renderer.cpp)

target_include_directories(pathTracer PRIVATE include)

find_package(embree 3 CONFIG REQUIRED)
target_link_libraries(pathTracer PRIVATE embree)