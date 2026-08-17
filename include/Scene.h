#pragma once
#include <vector>

#include "Box.h"
#include "Plane.h"
#include "Sphere.h"
#include "Model/Model.h"
#include "Model/Triangle.h"


enum class SelectedObject {
    None,
    Sphere,
    Box
};

class Scene {
public:
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;
    std::vector<Plane> planes;
    std::vector<Model> models;
    std::vector<Triangle> triangles;

    SelectedObject selectedObject = SelectedObject::None;
    int selectedIndex = -1;

    glm::vec3 skyColor = glm::vec3(0.5, 0.7, 1.0);
    float skyIntensity = 1.0;
    glm::vec3 skyHorizon = glm::vec3(0.75, 0.85, 1.0);

    void buildTriangles();
};

Scene randomScene(int seed);
Scene cornell();
