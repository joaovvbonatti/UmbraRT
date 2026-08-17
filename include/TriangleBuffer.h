#pragma once
#include "glad/gl.h"
#include "Model/Triangle.h"

class TriangleBuffer {
public:
    TriangleBuffer();
    ~TriangleBuffer();

    void upload(const std::vector<Triangle>& triangles);

    void bind(GLuint bindingPoint) const;
    size_t size() const;

private:
    GLuint UBO = 0;
    GLuint triangleCount = 0;
};
