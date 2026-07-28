#include "Renderer.h"

#include "glad/gl.h"

Renderer::Renderer()
    : shader("shaders/vertex.vert", "shaders/fragment.frag"),
    presentShader("shaders/vertex.vert", "shaders/present.frag")
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &sphereUBO);
}

void Renderer::createAccumBuffer(int width, int height) {
    if (accumFBO != 0) {
        glDeleteFramebuffers(1, &accumFBO);
        glDeleteTextures(1, &accumTexture);
    }

    accumWidth = width;
    accumHeight = height;

    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &accumFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Renderer: accumulation framebuffer is incomplete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    resetAccumulation();
}

void Renderer::resetAccumulation() {
    sampleCount = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Renderer::cameraChanged(const Camera& camera) const {
    if (!hasLastCamera) {
        return true;
    }

    const float eps = 1e-5f;

    if (glm::length(camera.position - lastCamera.position) > eps) return true;
    if (glm::length(camera.forward - lastCamera.forward) > eps) return true;
    if (glm::length(camera.right - lastCamera.right) > eps) return true;
    if (glm::length(camera.up - lastCamera.up) > eps) return true;
    if (std::fabs(camera.fov - lastCamera.fov) > eps) return true;

    return false;
}

void Renderer::render(Camera& camera, Scene& scene, int width, int height) {
    if (width != accumWidth || height != accumHeight) {
        createAccumBuffer(width, height);
    }

    if (cameraChanged(camera)) {
        resetAccumulation();
    }

    lastCamera = camera;
    hasLastCamera = true;

    sendToGPU(scene);

    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glViewport(0, 0, accumWidth, accumHeight);

    shader.use();
    shader.setVec2("uResolution", glm::vec2(width, height));
    shader.setVec3("uCameraPosition", camera.position);
    shader.setVec3("uCameraForward", camera.forward);
    shader.setVec3("uCameraRight", camera.right);
    shader.setVec3("uCameraUp", camera.up);
    shader.setFloat("uCameraFov", camera.fov);
    shader.setInt("uFrame", frameCount++);
    shader.setInt("uSphereCount", scene.getSphereCount());

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisable(GL_BLEND);

    sampleCount++;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    presentShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    presentShader.setInt("uAccum", 0);
    presentShader.setInt("uSampleCount", sampleCount);

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
