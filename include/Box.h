#pragma once
#include "Material.h"
#include "glm/vec3.hpp"

class Box {
public:
    glm::vec3 position;
    glm::vec3 size;

    Material material;
};
