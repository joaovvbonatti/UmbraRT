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


void Scene::addBox(glm::vec3 position, glm::vec3 size, glm::vec3 albedo, float emission) {
    boxes.push_back(Box(position, size, albedo, emission));
    boxCount++;
}

const int Scene::getBoxCount() const {
    return boxCount;
}
