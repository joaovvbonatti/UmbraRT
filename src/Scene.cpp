#include "Scene.h"

void Scene::addSphere(glm::vec3 position, glm::vec3 albedo, float radius, float emission) {
    spheres.push_back(Sphere(position, albedo, radius, emission));
    sphereCount++;
}

std::vector<Sphere>& Scene::getSpheres() {
    return spheres;
}

const int Scene::getSphereCount() const {
    return sphereCount;
}
