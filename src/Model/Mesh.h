#pragma once
#include <vector>

#include "Vertex.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
