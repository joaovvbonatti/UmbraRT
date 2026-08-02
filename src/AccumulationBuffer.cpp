#include "AccumulationBuffer.h"

#include <iostream>

AccumulationBuffer::AccumulationBuffer() = default;

AccumulationBuffer::~AccumulationBuffer()
{
    destroy();
}

void AccumulationBuffer::resize(int newWidth, int newHeight) {
    if (width == newWidth && height == newHeight)
        return;

    destroy();

    width = newWidth;
    height = newHeight;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA32F,
        width,
        height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        textureID,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Accumulation framebuffer is incomplete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    reset();
}

void AccumulationBuffer::reset() {
    sampleCount = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AccumulationBuffer::nextSample() {
    sampleCount++;
}

bool AccumulationBuffer::cameraChanged(const Camera &camera) {
    if (!hasLastCamera) {
        lastCamera = camera;
        hasLastCamera = true;
        return false;
    }

    if (camera != lastCamera) {
        lastCamera = camera;
        return true;
    }

    return false;
}

void AccumulationBuffer::destroy() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
}
