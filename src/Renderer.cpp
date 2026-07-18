#include "Renderer.h"

#include "glad/gl.h"

Renderer::Renderer() : shader("shaders/vertex.vert", "shaders/fragment.frag") {
    glGenVertexArrays(1, &VAO);
}

void Renderer::render(Camera camera, glm::vec3 spherePosition, int width, int height) {
    shader.setVec2("uResolution", glm::vec2(width, height));
    shader.setVec3("uSpherePosition", spherePosition);
    shader.setVec3("uCameraPosition", camera.position);
    shader.setVec3("uCameraForward", camera.forward);
    shader.setVec3("uCameraRight", camera.right);
    shader.setVec3("uCameraUp", camera.up);
    shader.setFloat("uCameraFov", camera.fov);
    shader.setInt("uFrame", frameCount++);

    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

Renderer::~Renderer() {
    glDeleteProgram(shader.id);
    glDeleteVertexArrays(1, &VAO);
}
