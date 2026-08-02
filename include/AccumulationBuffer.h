#pragma once
#include "Camera.h"
#include "glad/gl.h"

class AccumulationBuffer {
public:
    AccumulationBuffer();
    ~AccumulationBuffer();

    void resize(int newWidth, int newHeight);
    void reset();

    void nextSample();

    bool cameraChanged(const Camera& camera);

    GLuint texture() const { return textureID; };
    GLuint framebuffer() const { return FBO; };

    int getSampleCount() const { return sampleCount; };


private:
    void destroy();

    GLuint FBO = 0;
    GLuint textureID = 0;

    int width = 0;
    int height = 0;

    int sampleCount = 0;

    Camera lastCamera{};
    bool hasLastCamera = false;
};
