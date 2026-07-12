#include "Input.h"



Input::Input(GLFWwindow *window, Camera &camera) : window(window), camera(camera) {
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouseCallback);
}

void Input::mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (input)
        input->onMouseMove(xPos, yPos);
}

void Input::onMouseMove(double xPos, double yPos) {
    static bool firstMouse = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = static_cast<float>(xPos - lastX);
    float yOffset = static_cast<float>(lastY - yPos);

    lastX = xPos;
    lastY = yPos;

    camera.processMouse(xOffset, yOffset);
}

void Input::update(float deltaTime) {
    bool moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    camera.processKeyboard(moveForward, moveBackward, moveLeft, moveRight, deltaTime);
}
