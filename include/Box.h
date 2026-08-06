#pragma once
#include "glm/vec3.hpp"

class Box {
public:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 albedo;
    float emission;
};
