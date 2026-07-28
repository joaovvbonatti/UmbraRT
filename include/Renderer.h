#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "glad/gl.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(Camera& camera, Scene& scene, int width, int height);

private:
    unsigned int VAO = 0;
    Shader shader;
    Shader presentShader;
    int frameCount = 0;

    void sendToGPU(const Scene& scene);
    GLuint sphereUBO;

    GLuint accumFBO = 0;
    GLuint accumTexture = 0;
    int accumWidth = 0;
    int accumHeight = 0;
    int sampleCount = 0;

    Camera lastCamera{};
    bool hasLastCamera = false;

    void createAccumBuffer(int width, int height);
    void resetAccumulation();
    bool cameraChanged(const Camera& camera) const;
};
