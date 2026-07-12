#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    void updateVectors();

    void processKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime);

    void processMouse(float xOffset, float yOffset);

    glm::vec3 position;

    float fov = 60.f;

    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    float yaw;
    float pitch;

    float speed;
    float sensitivity;
};