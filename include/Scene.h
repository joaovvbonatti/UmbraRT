#pragma once
#include <vector>

#include "Box.h"
#include "Sphere.h"

class Scene {
public:
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;

    int selectedSphere = -1;
};
