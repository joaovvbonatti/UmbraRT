#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "Camera.h"
#include "DebugUI.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"
#include "Scene.h"

int main() {
    Window window(1600, 900);

    Camera camera;

    Input input(window.getHandle(), camera);

    DebugUI ui(window.getHandle());

    float lastFrame = glfwGetTime();

    Renderer renderer;

    Scene scene;
    scene.addSphere(glm::vec3(3, 1, 3), glm::vec3(1.0), 1.0, 0.0);
    scene.addSphere(glm::vec3(0, 2, -4), glm::vec3(1.0), 2.0, 0.0);
    scene.addSphere(glm::vec3(0, 2, 2), glm::vec3(1.0, 0.0, 0.0), 0.5, 0.0);
    scene.addSphere(glm::vec3(0, 5, 0), glm::vec3(1.0), 0.3, 5.0);

    while (!window.shouldClose()) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        input.update(deltaTime);

        renderer.render(camera, scene);

        bool changed = false;

        ui.beginFrame();
        changed |= ui.drawCameraPanel(camera);
        changed |= ui.drawScenePanel(scene);
        changed |= ui.drawRenderPanel(renderer);
        ui.drawViewport(renderer);
        ui.endFrame();

        if (changed) {
            renderer.resetAccumulation();
        }

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}