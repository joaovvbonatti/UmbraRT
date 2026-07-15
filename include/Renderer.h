#pragma once
#include "Camera.h"
#include "Shader.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(Camera camera, glm::vec3 spherePosition, int width, int height);
private:
    unsigned int VAO = 0;
    Shader shader;
};
