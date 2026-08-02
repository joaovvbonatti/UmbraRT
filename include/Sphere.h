#pragma once
#include "glm/vec3.hpp"

class Sphere {
public:
    Sphere(const glm::vec3 &position, const glm::vec3 &albedo, float radius, float emission);

    const glm::vec3& getPosition() const;
    void setPosition(glm::vec3 position);

    const glm::vec3& getAlbedo() const;
    void setAlbedo(glm::vec3 albedo);

    float getRadius() const;
    void setRadius(float radius);

    float getEmission() const;
    void setEmission(float emission);

    glm::vec3 position;
    glm::vec3 albedo;
    float radius;
    float emission;
};
