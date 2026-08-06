#pragma once
#include <vector>

#include "Box.h"
#include "Sphere.h"

class Scene {
public:
    void addSphere(glm::vec3 position, glm::vec3 albedo, float radius, float emission);
    std::vector<Sphere>& getSpheres();
    const int getSphereCount() const;

    void addBox(glm::vec3 position, glm::vec3 size, glm::vec3 albedo, float emission);
    std::vector<Box> boxes;
    const int getBoxCount() const;

    int selectedSphere = -1;

private:
    std::vector<Sphere> spheres;

    int sphereCount = 0;
    int boxCount = 0;
};
