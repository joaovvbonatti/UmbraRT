#include "Sphere.h"

Sphere::Sphere(const glm::vec3 &position, const glm::vec3 &albedo, float radius, float emission)
        : position(position),
          albedo(albedo),
          radius(radius),
          emission(emission) {
}

const glm::vec3& Sphere::getPosition() const {
    return position;
}

void Sphere::setPosition(glm::vec3 position) {
    this->position = position;
}

const glm::vec3& Sphere::getAlbedo() const {
    return albedo;
}

void Sphere::setAlbedo(glm::vec3 albedo) {
    this->albedo = albedo;
}

float Sphere::getRadius() const {
    return radius;
}

void Sphere::setRadius(float radius) {
    this->radius = radius;
}

float Sphere::getEmission() const {
    return emission;
}

void Sphere::setEmission(float emission) {
    this->emission = emission;
}