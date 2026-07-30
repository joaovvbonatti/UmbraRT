#pragma once
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "GLFW/glfw3.h"

class DebugUI {
public:
    DebugUI(GLFWwindow* window);
    ~DebugUI();

    void beginFrame();
    void render(Camera& camera);
    void endFrame();

    void drawScenePanel(Scene& scene);
    void drawCameraPanel(Camera& camera);
    void drawRenderPanel(Renderer& renderer);
    void drawViewport(Renderer& renderer);
};
