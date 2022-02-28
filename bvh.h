//
// Created by jiahaoww on 2022/2/24.
//

#ifndef PATHTRACER_BVH_H
#define PATHTRACER_BVH_H

#include "object.h"
#include "aabb.h"
#include <utility>
#include <vector>
#include <iostream>

struct BvhNode {
    AABB bounding_box;
    Object *obj = nullptr;
    BvhNode *left = nullptr;
    BvhNode *right = nullptr;
    float area = 0.0f;
    size_t size = 0;
};

class BVH {
public:
    BvhNode *root;
    std::vector<Object *> objects;
public:
    BvhNode *recursive_build(Object **objs, size_t length) {
        if (length == 1) {
            auto node = new BvhNode;
            node->bounding_box = objs[0]->bounding_box;
            node->obj = objs[0];
            node->left = nullptr;
            node->right = nullptr;
            node->area = objs[0]->get_area();
            node->size = 1;
            return node;
        } else if (length == 2) {
            auto node = new BvhNode;

            auto left = recursive_build(objs, 1);
            auto right = recursive_build(objs + 1, 1);
            node->bounding_box = AABB::Union(left->bounding_box, right->bounding_box);
            node->left = left;
            node->right = right;
            node->area = left->area + right->area;
            node->size = 2;
            return node;
        } else {
            auto node = new BvhNode;
            AABB bounding_box;
            for (int i = 0; i < length; i++) {
                bounding_box.Union(objs[i]->bounding_box);
            }
            vec3 extent = bounding_box.extent();
            float max_dimension = std::max({extent.x, extent.y, extent.z});
            if (extent.x == max_dimension) {
                std::sort(objs, objs + length, [](auto &&obj1, auto &&obj2) {
                    return obj1->bounding_box.center().x < obj2->bounding_box.center().x;
                });
            } else if (extent.y == max_dimension) {
                std::sort(objs, objs + length, [](auto &&obj1, auto &&obj2) {
                    return obj1->bounding_box.center().y < obj2->bounding_box.center().y;
                });
            } else {
                std::sort(objs, objs + length, [](auto &&obj1, auto &&obj2) {
                    return obj1->bounding_box.center().z < obj2->bounding_box.center().z;
                });
            }
            node->bounding_box = bounding_box;
            auto left = recursive_build(objs, length / 2);
            auto right = recursive_build(objs + length / 2, length - length / 2);
            node->left = left;
            node->right = right;
            node->area = left->area + right->area;
            node->size = length;
            return node;
        }
    }


    BVH(const std::vector<Object *> &objs) {
        objects = objs;
        std::cout << "start building BVH" << std::endl;
        root = recursive_build(objects.data(), objects.size());
        // print_aabb(root);
    }

    Intersection get_intersection(BvhNode *node, const Ray &ray) {
        Intersection inter;
        if (!node->bounding_box.intersect(ray)) {
            return inter;
        }

        if (node->left == nullptr && node->right == nullptr) {
            return node->obj->get_interaction(ray);
        } else {
            Intersection inter_left = get_intersection(node->left, ray);
            Intersection inter_right = get_intersection(node->right, ray);
            if (inter_left.t < inter_right.t) {
                return inter_left;
            } else {
                return inter_right;
            }
        }
    }

    Intersection intersect(const Ray &ray) {
        Intersection inter;
        if (root == nullptr) {
            return inter;
        }
        return get_intersection(root, ray);
    }

    void sample_node(BvhNode *node, Intersection &inter, float &pdf, float p, float s) {
        if (node->left == nullptr && node->right == nullptr) {
            node->obj->sample(inter, pdf);
            pdf *= node->obj->get_area();
        } else {
            if (p < node->left->area) {
                sample_node(node->left, inter, pdf, p, s);
            } else {
                sample_node(node->right, inter, pdf, p - node->left->area, s);
            }
        }
    }

    void sample(Intersection &inter, float &pdf) {
        float p = std::sqrt(get_random_float()) * root->area;
        sample_node(root, inter, pdf, p, root->area);
        pdf /= root->area;
    }

    void print_aabb(BvhNode* node) {
        if (node == nullptr) {
            return;
        }
        print_aabb(node->left);
        std::cout << "size " << node->size << std::endl;
        std::cout << "area " << node->area << std::endl;
        std::cout << "min " << node->bounding_box.v_min.x << " " << node->bounding_box.v_min.y << " " << node->bounding_box.v_min.z << std::endl;
        std::cout << "max " << node->bounding_box.v_max.x << " " << node->bounding_box.v_max.y << " " << node->bounding_box.v_max.z << std::endl;
        std::cout << std::endl;
        print_aabb(node->right);
    }

};

#endif //PATHTRACER_BVH_H
