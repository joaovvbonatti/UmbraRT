#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
    position = glm::vec3(0.f, 0.f, 2.f);

    yaw = -90.f;
    pitch = 0.f;
    fov = 60.f;
    speed = 3.f;
    sensitivity = 0.1f;

    updateVectors();
}

void Camera::updateVectors() {
    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

    forward.y = sin(glm::radians(pitch));

    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    forward = glm::normalize(forward);

    right = glm::normalize(glm::cross(forward, glm::vec3(0.f,1.f,0.f)));

    up = glm::normalize(glm::cross(right,forward));
}

void Camera::processMouse(float dx,float dy) {
    yaw += dx * sensitivity;

    pitch += dy * sensitivity;

    pitch = glm::clamp(pitch, -89.f, 89.f);

    updateVectors();
}

void Camera::processKeyboard(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float dt) {
    float velocity = speed * dt;

    if(moveForward)
        position += forward * velocity;

    if(moveBackward)
        position -= forward * velocity;

    if(moveLeft)
        position -= right * velocity;

    if(moveRight)
        position += right * velocity;
}