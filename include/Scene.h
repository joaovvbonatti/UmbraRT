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

    glm::vec3 skyColor = glm::vec3(0.5, 0.7, 1.0);
    float skyIntensity = 1.0;
    glm::vec3 skyHorizon = glm::vec3(0.75, 0.85, 1.0);

};

Scene randomScene(int seed);
