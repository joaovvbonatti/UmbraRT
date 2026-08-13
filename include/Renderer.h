#pragma once

#include "AccumulationBuffer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "glad/gl.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(Camera& camera, Scene& scene);

    GLuint getRenderTexture();
    void setViewportSize(int width, int height);
    int viewportWidth = 1;
    int viewportHeight = 1;

    void resetAccumulation();
private:
    unsigned int VAO = 0;
    Shader shader;
    Shader presentShader;
    int frameCount = 0;

    void sendToGPU(Scene& scene);
    GLuint sphereUBO;
    GLuint boxUBO;
    GLuint planeUBO;

    GLuint presentFBO = 0;
    GLuint presentTexture = 0;
    int presentWidth = 0;
    int presentHeight = 0;

    AccumulationBuffer accumulation;

    void createPresentBuffer(int width, int height);
};
