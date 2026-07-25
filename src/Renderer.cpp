#include "Renderer.h"

#include "glad/gl.h"

Renderer::Renderer() : shader("shaders/vertex.vert", "shaders/fragment.frag") {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &sphereUBO);
}

void Renderer::render(Camera camera, Scene scene, int width, int height) {
    shader.setVec2("uResolution", glm::vec2(width, height));
    shader.setVec3("uCameraPosition", camera.position);
    shader.setVec3("uCameraForward", camera.forward);
    shader.setVec3("uCameraRight", camera.right);
    shader.setVec3("uCameraUp", camera.up);
    shader.setFloat("uCameraFov", camera.fov);
    shader.setInt("uFrame", frameCount++);
    shader.setInt("uSphereCount", scene.getSphereCount());

    sendToGPU(scene);

    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

struct gpuSphere {
    glm::vec4 positionRadius;
    glm::vec4 albedoEmission;
};

void Renderer::sendToGPU(const Scene &scene) {
    std::vector<gpuSphere> gpuSpheres;

    for (const Sphere& sphere : scene.getSpheres()) {
        gpuSphere data;

        data.positionRadius = glm::vec4(sphere.getPosition(), sphere.getRadius());
        data.albedoEmission = glm::vec4(sphere.getAlbedo(), sphere.getEmission());

        gpuSpheres.push_back(data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER,sphereUBO);
    glBufferData(GL_UNIFORM_BUFFER, gpuSpheres.size() * sizeof(gpuSphere), gpuSpheres.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,sphereUBO);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}

Renderer::~Renderer() {
    glDeleteProgram(shader.id);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &sphereUBO);
}
