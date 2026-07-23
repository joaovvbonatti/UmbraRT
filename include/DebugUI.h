#pragma once
#include "Camera.h"
#include "GLFW/glfw3.h"

class DebugUI {
public:
    DebugUI(GLFWwindow* window);
    ~DebugUI();

    void beginFrame();
    void render(Camera& camera, glm::vec3& spherePos);
};
