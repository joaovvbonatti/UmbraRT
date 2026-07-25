#pragma once
#include "GLFW/glfw3.h"

class Window {
public:
    Window(int width, int height);
    ~Window();

    bool shouldClose() const;

    GLFWwindow* getHandle() const;
    void pollEvents() const;
    void swapBuffers();

    int getWidth();
    int getHeight();

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    GLFWwindow* handle = nullptr;

    int width, height;

};
