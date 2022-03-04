//
// Created by vr on 2022/2/28.
//

#ifndef PATHTRACER_XML_H
#define PATHTRACER_XML_H
#include <map>
#include <string>
#include "global.h"
#include "xml/pugixml.cpp"
#include <sstream>
#include "camera.h"

vec3 string2vec3(std::string& str) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ',') {
            str[i] = ' ';
        }
    }
    std::stringstream ss(str);
    float a[3];
    int j = 0;
    float tmp;
    while(ss >> tmp) {
        a[j++] = tmp;
    }
    return vec3(a[0], a[1], a[2]);
}

class XML {
public:
    static std::map<std::string, vec3> load_xml(std::string& xml_path, Camera& cam) {
        std::map <std::string, vec3> m;
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file(xml_path.c_str());
        if (!res) {
            return m;
        }
        for (pugi::xml_node node = doc.first_child(); node; node = node.next_sibling()) {
            std::string node_name = node.name();
            if (node_name == "camera") {
                pugi::xml_node camera = node;
                std::string eye_str = camera.child("eye").attribute("value").value();
                vec3 eye = string2vec3(eye_str);

                std::string look_at_str = camera.child("lookat").attribute("value").value();
                vec3 look_at = string2vec3(look_at_str);

                std::string up_str = camera.child("up").attribute("value").value();
                vec3 up = string2vec3(up_str);

                float fov = camera.child("fovy").attribute("value").as_float();
                int width = camera.child("width").attribute("value").as_int();
                int height = camera.child("height").attribute("value").as_int();

                cam.width = width;
                cam.height = height;
                cam.fov = fov;
                cam.eye = eye;
                cam.look_at = look_at;
                cam.up = up;
            }
            if (node_name == "light") {
                std::string mtl_name = node.attribute("mtlname").value();
                std::string radiance_str = node.attribute("radiance").value();
                vec3 radiance = string2vec3(radiance_str);
                m[mtl_name] = radiance;
            }
        }
        return m;
    }
};

#endif //PATHTRACER_XML_H
