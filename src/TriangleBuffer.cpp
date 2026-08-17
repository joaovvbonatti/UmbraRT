#include "TriangleBuffer.h"

#include "glm/vec4.hpp"

TriangleBuffer::TriangleBuffer() {
    glGenBuffers(1, &UBO);
}

TriangleBuffer::~TriangleBuffer() {
    if (UBO != 0)
        glDeleteBuffers(1, &UBO);
}

struct gpuTriangle {
    glm::vec4 v0;
    glm::vec4 v1;
    glm::vec4 v2;

    glm::vec4 n0;
    glm::vec4 n1;
    glm::vec4 n2;
};

void TriangleBuffer::upload(const std::vector<Triangle> &triangles) {
    std::vector<gpuTriangle> gpuTriangles;
    gpuTriangles.reserve(triangles.size());

    for (const Triangle& triangle : triangles) {
        gpuTriangle data;

        data.v0 = glm::vec4(triangle.v0, 1.0f);
        data.v1 = glm::vec4(triangle.v1, 1.0f);
        data.v2 = glm::vec4(triangle.v2, 1.0f);

        data.n0 = glm::vec4(triangle.n0, 0.0f);
        data.n1 = glm::vec4(triangle.n1, 0.0f);
        data.n2 = glm::vec4(triangle.n2, 0.0f);

        gpuTriangles.push_back(data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, gpuTriangles.size() * sizeof(gpuTriangle), gpuTriangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    triangleCount = triangles.size();
}

void TriangleBuffer::bind(GLuint bindingPoint) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
}

size_t TriangleBuffer::size() const {
    return triangleCount;
}
