//
// Created by jiahaoww on 2022/2/24.
//

#include "mesh.h"

Mesh::Mesh(const std::vector<Object *>& objects) {
    prims = objects;
    bvh = new BVH(objects);
    bounding_box = bvh->root->bounding_box;
    m = objects[0]->m;
}

float Mesh::get_area() {
    return bvh->root->area;
}

Intersection Mesh::get_interaction(const Ray &ray) {
    return bvh->intersect(ray);
}

void Mesh::sample(Intersection &inter, float &pdf) {
    bvh->sample(inter, pdf);
}
