#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "glad/gl.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(Camera camera, Scene scene, int width, int height);

private:
    unsigned int VAO = 0;
    Shader shader;
    int frameCount = 0;

    void sendToGPU(const Scene& scene);
    GLuint sphereUBO;
};
