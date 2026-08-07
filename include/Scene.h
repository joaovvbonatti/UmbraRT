#pragma once
#include <vector>

#include "Box.h"
#include "Sphere.h"

enum class SelectedObject {
    None,
    Sphere,
    Box
};

class Scene {
public:
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;

    SelectedObject selectedObject = SelectedObject::None;
    int selectedIndex = -1;
};
