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

    bool drawScenePanel(Scene& scene);
    bool drawCameraPanel(Camera& camera);
    bool drawRenderPanel(Renderer& renderer);
    void drawViewport(Renderer& renderer);
    bool drawSkyPanel(Scene& scene);
};
