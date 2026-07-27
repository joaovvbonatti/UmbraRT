#pragma once
#include <vector>
#include "Sphere.h"

class Scene {
public:
    void addSphere(glm::vec3 position, glm::vec3 albedo, float radius, float emission);
    const std::vector<Sphere> getSpheres() const;
    const int getSphereCount() const;

private:
    std::vector<Sphere> spheres;
    int sphereCount = 0;
};
