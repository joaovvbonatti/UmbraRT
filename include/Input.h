#pragma once
#include "Camera.h"
#include "GLFW/glfw3.h"

class Input {
public:
    Input(GLFWwindow* window, Camera& camera);

    void update(float deltaTime);
private:
    static void mouseCallback(GLFWwindow*, double xPos, double yPos);

    Camera& camera;
    GLFWwindow* window;

    void onMouseMove(double xPos, double yPos);

    bool mouseCaptured = true;
    bool tabPressed = false;

    bool firstMouse = true;
    double lastX = 0.0;
    double lastY = 0.0;

    void setMouseCaptured(bool captured);
};
