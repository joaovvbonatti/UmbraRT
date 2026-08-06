#pragma once
#include "glm/vec3.hpp"

class Sphere {
public:
    glm::vec3 position;
    glm::vec3 albedo;
    float radius;
    float emission;
};
