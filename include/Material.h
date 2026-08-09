#pragma once
#include "glm/vec3.hpp"

enum MaterialType {
    MATERIAL_DIFFUSE = 0,
    MATERIAL_METAL,
    MATERIAL_DIELECTRIC,
    MATERIAL_EMISSIVE
};

class Material {
public:
    int type = MATERIAL_DIFFUSE;

    glm::vec3 albedo = glm::vec3(1.0);

    float roughness = 0.0f;
    float emission = 0.0f;
    float ior = 1.5f;
};