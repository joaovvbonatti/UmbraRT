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

    ImGui::Begin("Scene");

    if (ImGui::Button("Add"))
        ImGui::OpenPopup("AddObject");

    if (ImGui::BeginPopup("AddObject"))
    {
        if (ImGui::MenuItem("Sphere")) {
            scene.spheres.emplace_back(glm::vec3(0, 1, 0), 1.0, Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 0.0, 0.0));
            changed = true;
        }

        if (ImGui::MenuItem("Box")) {
            scene.boxes.emplace_back(glm::vec3(0, 1, 0), glm::vec3(1), Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 0.0, 0.0));
            changed = true;
        }

        ImGui::EndPopup();
    }

    if (ImGui::CollapsingHeader("Spheres"))
    {
        for (int i = 0; i < scene.spheres.size(); i++)
        {
            bool selected = scene.selectedObject == SelectedObject::Sphere && scene.selectedIndex == i;

            if (ImGui::Selectable(("Sphere " + std::to_string(i)).c_str(), selected))
            {
                scene.selectedObject = SelectedObject::Sphere;
                scene.selectedIndex = i;
            }
        }
    }

    if (ImGui::CollapsingHeader("Boxes"))
    {
        for (int i = 0; i < scene.boxes.size(); i++)
        {
            bool selected =
                scene.selectedObject == SelectedObject::Box && scene.selectedIndex == i;

            if (ImGui::Selectable(("Box " + std::to_string(i)).c_str(), selected))
            {
                scene.selectedObject = SelectedObject::Box;
                scene.selectedIndex = i;
            }
        }
    }

    ImGui::Begin("Editor");

    switch (scene.selectedObject)
    {
        case SelectedObject::Sphere:
        {
            Sphere& sphere = scene.spheres[scene.selectedIndex];

            changed |= ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
            changed |= ImGui::DragFloat("Radius", &sphere.radius, 0.01f, 0.0f, FLT_MAX);
            changed |= ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.material.albedo));
            changed |= ImGui::DragFloat("Emission", &sphere.material.emission, 0.1f, 0.0f, FLT_MAX);

            //Material specific settings
            int material = static_cast<int>(sphere.material.type);

            if (ImGui::Combo("Material",&material,Material::names,IM_ARRAYSIZE(Material::names))) {
                sphere.material.type = static_cast<Material::MaterialType>(material);
                changed = true;
            }

            if (material == Material::METAL)
                changed |= ImGui::DragFloat("Roughness", &sphere.material.roughness, 0.01f, 0.0f, FLT_MAX);

            break;
        }

        case SelectedObject::Box:
        {
            Box& box = scene.boxes[scene.selectedIndex];

            changed |= ImGui::DragFloat3("Position", glm::value_ptr(box.position), 0.1f);
            changed |= ImGui::DragFloat3("Size", glm::value_ptr(box.size), 0.1f, 0.0f, FLT_MAX);
            changed |= ImGui::ColorEdit3("Albedo", glm::value_ptr(box.material.albedo));
            changed |= ImGui::DragFloat("Emission", &box.material.emission, 0.1f, 0.0f, FLT_MAX);

            //Material specific settings
            int material = static_cast<int>(box.material.type);

            if (ImGui::Combo("Material",&material,Material::names,IM_ARRAYSIZE(Material::names))) {
                box.material.type = static_cast<Material::MaterialType>(material);
                changed = true;
            }

            if (material == Material::METAL)
                changed |= ImGui::DragFloat("Roughness", &box.material.roughness, 0.01f, 0.0f, 1.0f);

            break;
        }

        default:
            break;
    }

    ImGui::End();

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

bool DebugUI::drawSkyPanel(Scene& scene) {
    bool changed = false;

    ImGui::Begin("Sky");

    changed |= ImGui::ColorEdit3("Sky color", glm::value_ptr(scene.skyColor));
    changed |= ImGui::ColorEdit3("Horizon color", glm::value_ptr(scene.skyHorizon));
    changed |= ImGui::DragFloat("Sky intensity", &scene.skyIntensity, 0.01);

    ImGui::End();

    return changed;
}

void DebugUI::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
