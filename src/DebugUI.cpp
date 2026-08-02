#include "DebugUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

DebugUI::DebugUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

DebugUI::~DebugUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
}


bool DebugUI::drawScenePanel(Scene &scene) {
    bool changed = false;

    std::vector<Sphere>& spheres = scene.getSpheres();

    ImGui::Begin("Scene");

    if (ImGui::Button("Add sphere")) {
        scene.addSphere(glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0), 1.0, 0.0);
        changed = true;
    }


    for (int i = 0; i < spheres.size(); i++) {
        if (ImGui::Selectable(("Sphere " + std::to_string(i)).c_str(), scene.selectedSphere == i))
            scene.selectedSphere = i;
    }

    std::cout << scene.selectedSphere << std::endl;

    if (scene.selectedSphere != -1) {
        ImGui::Begin("Editor");
        changed |= ImGui::DragFloat3("Position", glm::value_ptr(spheres[scene.selectedSphere].position), 0.1);
        changed |= ImGui::DragFloat("Radius", &spheres[scene.selectedSphere].radius, 0.01);
        changed |= ImGui::ColorPicker3("Albedo", glm::value_ptr(spheres[scene.selectedSphere].albedo));
        changed |= ImGui::DragFloat("Emission", &spheres[scene.selectedSphere].emission, 0.1);
        ImGui::End();
    }

    ImGui::End();

    return changed;
}

bool DebugUI::drawCameraPanel(Camera &camera) {
    bool changed = false;

    ImGui::Begin("Camera");

    changed |= ImGui::DragFloat("FoV", &camera.fov, 0.1);
    changed |= ImGui::DragFloat3("Position", glm::value_ptr(camera.position), 0.1);
    changed |= ImGui::DragFloat("Pitch", &camera.pitch, 0.1);
    changed |= ImGui::DragFloat("Yaw", &camera.yaw, 0.1);

    camera.updateVectors();

    ImGui::End();

    return changed;
}

bool DebugUI::drawRenderPanel(Renderer &renderer) {
    bool changed;

    ImGui::Begin("Renderer");

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Viewport width: %i", renderer.viewportWidth);
    ImGui::Text("Viewport height: %i", renderer.viewportHeight);
    ImGui::End();

    return false;
}

void DebugUI::drawViewport(Renderer& renderer) {
    ImGui::Begin("Viewport");

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    renderer.setViewportSize(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));

    ImGui::Image((ImTextureID)(intptr_t)renderer.getRenderTexture(), viewportSize,ImVec2(0.0f, 1.0f),ImVec2(1.0f, 0.0f));

    ImGui::End();

}

void DebugUI::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
