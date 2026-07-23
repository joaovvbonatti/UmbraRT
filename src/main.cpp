#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "Camera.h"
#include "DebugUI.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"

int main() {
    Window window(1600, 900);

    Camera camera;

    Input input(window.getHandle(), camera);

    DebugUI ui(window.getHandle());

    glm::vec3 spherePosition(0.0);

    float lastFrame = glfwGetTime();

    Renderer renderer;

    while (!window.shouldClose()) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        input.update(deltaTime);

        renderer.render(camera, spherePosition, window.getWidth(), window.getHeight());

        ui.beginFrame();
        ui.render(camera, spherePosition);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}