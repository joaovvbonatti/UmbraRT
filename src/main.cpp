#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DebugUI.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"
#include "Scene.h"

int main() {
    Window window(1600, 900);

    Camera camera;
    camera.position = glm::vec3(-10.0, 5.0, 0.0);
    camera.yaw = 0.0f;

    Input input(window.getHandle(), camera);

    DebugUI ui(window.getHandle());

    float lastFrame = glfwGetTime();

    Renderer renderer;

    Scene scene = cornell();
    //Scene scene = randomScene(999);
    //scene.planes.emplace_back(glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0), Material(Material::DIFFUSE, glm::vec3(0.1), 0.0, 0.0, 0.0));

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
        changed |= ui.drawSkyPanel(scene);
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