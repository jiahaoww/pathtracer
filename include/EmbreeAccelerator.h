#ifndef TOYS_EMBREE_ACCELERATOR_H
#define TOYS_EMBREE_ACCELERATOR_H

#include <embree3/rtcore.h>
#include <vector>
#include <unordered_map>

#include "triangle.h"

class EmbreeAccelerator {
    std::unordered_map<unsigned, Triangle*> m_faces;
    RTCDevice m_device;
    RTCScene m_scene;
    bool m_init_flag = false;
    float t_near = 1e-2;

public:
    EmbreeAccelerator() {
        m_device = rtcNewDevice(nullptr);
        m_scene = rtcNewScene(m_device);
    }

    ~EmbreeAccelerator() {
    }

    bool has_init() const noexcept {
        return m_init_flag;
    }

    void add_faces(std::vector<Triangle*>& faces) {
        for (int i = 0; i < faces.size(); ++i) {
            RTCGeometry geometry = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
            auto *vb = (glm::vec3 *) rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0,
                                                             RTC_FORMAT_FLOAT3, sizeof(glm::vec3), 3);
            auto *ib = (glm::uvec3 *) rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0,
                                                              RTC_FORMAT_UINT3, sizeof(glm::uvec3), 1);

            auto face = faces[i];
            vb[0] = face->v[0];
            vb[1] = face->v[1];
            vb[2] = face->v[2];
            ib[0] = {0, 1, 2};

            rtcCommitGeometry(geometry);
            m_faces[rtcAttachGeometry(m_scene, geometry)] = face;
        }
    }

    void init() {
        rtcCommitScene(m_scene);
        m_init_flag = true;
    }

    void clear() {
        m_faces.clear();
        m_init_flag = false;
        rtcCommitScene(m_scene);
    }

    Intersection hit(const Ray &ray) const noexcept {
        Intersection inter;
        RTCRayHit result{};
        result.ray.org_x = ray.ori.x;
        result.ray.org_y = ray.ori.y;
        result.ray.org_z = ray.ori.z;
        result.ray.dir_x = ray.dir.x;
        result.ray.dir_y = ray.dir.y;
        result.ray.dir_z = ray.dir.z;
        result.ray.tnear = 1e-3;
        result.ray.tfar = std::numeric_limits<float>::infinity();
        result.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        RTCIntersectContext context{};
        rtcInitIntersectContext(&context);
        rtcIntersect1(m_scene, &context, &result);

        if (result.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            Triangle* face = m_faces.at(result.hit.geomID);
            inter.has = true;
            // inter.normal = face->normal;
            inter.normal = glm::normalize(result.hit.u * face->vn[1] + result.hit.v * face->vn[2] + (1.0f - result.hit.u - result.hit.v) * face->vn[0]);
            inter.obj = face;
            inter.m = face->m;
            inter.pos = ray.ori + ray.dir * result.ray.tfar;;
            inter.t = result.ray.tfar;
            inter.b1 = result.hit.u;
            inter.b2 = result.hit.v;
        }
        return inter;
    }
};

#endif //TOYS_EMBREE_ACCELERATOR_H
