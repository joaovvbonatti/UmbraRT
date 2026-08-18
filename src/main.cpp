#include <iostream>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DebugUI.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"
#include "Scene.h"
#include "glm/ext/matrix_transform.hpp"
#include "Model/ModelLoader.h"

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

    //Model cube = ModelLoader::load("assets/cube.obj");
    //cube.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.0f, 0.0f));
    //scene.models.push_back(std::move(cube));

    Model suzanne = ModelLoader::load("assets/suzanne-0.3.obj");
    glm::vec3 position(-3.0f, 3.0f, -0.0f);
    glm::vec3 scale(2.0f);
    suzanne.transform =
    glm::translate(glm::mat4(1.0f), position)
    * glm::rotate(
        glm::mat4(1.0f),
        glm::radians(-90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    )
    * glm::scale(glm::mat4(1.0f), scale);

    scene.models.push_back(std::move(suzanne));
    scene.buildTriangles();
    std::cout << "Triangles: " << scene.triangles.size() << std::endl;

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
