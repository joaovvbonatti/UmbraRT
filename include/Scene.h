#pragma once
#include <vector>
#include "Sphere.h"

class Scene {
public:
    void addSphere(glm::vec3 position, glm::vec3 albedo, float radius, float emission);
    std::vector<Sphere>& getSpheres();
    const int getSphereCount() const;

    void sendToGPU();

    int selectedSphere = -1;

private:
    std::vector<Sphere> spheres;

    int sphereCount = 0;
};
