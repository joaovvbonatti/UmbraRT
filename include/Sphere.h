#pragma once
#include "glm/vec3.hpp"
#include "Material.h"

class Sphere {
public:
    glm::vec3 position;
    float radius;

    Material material;
};
