#pragma once
#include "glm/vec3.hpp"



class Material {
public:
    enum MaterialType {
        DIFFUSE = 0,
        METAL,
        DIELECTRIC,
    };

    static constexpr const char* names[] = {
        "Diffuse",
        "Metal",
        "Dielectric"
    };

    int type = DIFFUSE;

    glm::vec3 albedo = glm::vec3(1.0);

    float roughness = 0.0f;
    float emission = 0.0f;
    float ior = 1.5f;
};