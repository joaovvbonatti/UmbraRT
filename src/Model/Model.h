#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>

class Model {
public:
    std::vector<Mesh> meshes;

    glm::mat4 transform {1.0f};
};
