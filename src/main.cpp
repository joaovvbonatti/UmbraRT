#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>

#include "Shader.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

bool firstMouse = true;
double lastX = 0;
double lastY = 0;

bool mouseCaptured = true;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!mouseCaptured)
        return;

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    static bool firstMouse = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos - lastX);
    float yOffset = static_cast<float>(lastY - ypos);

    lastX = xpos;
    lastY = ypos;

    camera->processMouse(xOffset, yOffset);
}

int width = 1600;
int height = 900;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Camera& camera, float deltaTime) {
    static bool tabPressed = false;

    bool tab = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

    if (tab && !tabPressed)
    {
        mouseCaptured = !mouseCaptured;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            mouseCaptured
                ? GLFW_CURSOR_DISABLED
                : GLFW_CURSOR_NORMAL);
    }

    tabPressed = tab;


    if (!mouseCaptured)
        return;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(true, false, false, false, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(false, true, false, false, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(false, false, true, false, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(false, false, false, true, deltaTime);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "UmbraRT", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera camera;

    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouseCallback);

    //Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glViewport(0, 0, width, height);

    Shader shader("shaders/vertex.vert", "shaders/fragment.frag");

    float vertices[] =
    {
        -1.f,-1.f,0.f,
         1.f,-1.f,0.f,
        -1.f, 1.f,0.f,
         1.f, 1.f,0.f
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec3 spherePosition(0.0);

    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, camera, deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Debug");
        ImGui::DragFloat3("Sphere position", glm::value_ptr(spherePosition), 0.01f);
        ImGui::DragFloat("Camera FOV", &camera.fov, 0.1f);
        ImGui::End();

        shader.use();

        glfwGetFramebufferSize(window, &width, &height);

        shader.setVec2("uResolution", glm::vec2(width, height));
        shader.setVec3("uSpherePosition", spherePosition);
        shader.setVec3("uCameraPosition", camera.position);
        shader.setVec3("uCameraForward", camera.forward);
        shader.setVec3("uCameraRight", camera.right);
        shader.setVec3("uCameraUp", camera.up);
        shader.setFloat("uCameraFov", camera.fov);


        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader.id);
    glfwTerminate();
    return 0;
}
