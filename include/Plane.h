#pragma once
#include "Material.h"
#include "glm/vec3.hpp"

class Plane {
public:
    glm::vec3 point;
    glm::vec3 normal;

    Material material;
};
