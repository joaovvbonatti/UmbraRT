#include "Window.h"

#include <cstdlib>

#include "glad/gl.h"

Window::Window(int width, int height) : width(width), height(height) {
    if (!glfwInit())
        std::exit(1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(width, height, "UmbraRT", nullptr, nullptr);
    if (handle == nullptr)
        std::exit(1);

    glfwMakeContextCurrent(handle);
    glfwSetFramebufferSizeCallback(handle, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress))
        std::exit(1);

    glfwGetFramebufferSize(handle, &width, &height);
    glViewport(0, 0, width, height);
}

GLFWwindow* Window::getHandle() const {
    return handle;
}

void Window::pollEvents() const {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(handle);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

int Window::getWidth() {
    glfwGetFramebufferSize(handle, &width, &height);
    return width;
}

int Window::getHeight() {
    glfwGetFramebufferSize(handle, &width, &height);
    return height;
}

Window::~Window() {
    if (handle != nullptr) {
        glfwDestroyWindow(handle);
        handle = nullptr;
    }
    glfwTerminate();
}
