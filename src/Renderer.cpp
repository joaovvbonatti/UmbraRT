#include "Renderer.h"

#include <iostream>

#include "glad/gl.h"

Renderer::Renderer()
    : shader("shaders/vertex.vert", "shaders/fragment.frag"),
    presentShader("shaders/vertex.vert", "shaders/present.frag")
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &sphereUBO);
    glGenBuffers(1, &boxUBO);
    glGenBuffers(1, &planeUBO);
}

void Renderer::render(Camera& camera, Scene& scene) {
    accumulation.resize(viewportWidth, viewportHeight);

    if (accumulation.cameraChanged(camera)) {
        frameCount = 0;
        accumulation.reset();
    }

    sendToGPU(scene);

    glBindFramebuffer(GL_FRAMEBUFFER,accumulation.framebuffer());

    glViewport(0,0, viewportWidth, viewportHeight);

    shader.use();

    shader.setVec2("uResolution",glm::vec2(viewportWidth, viewportHeight));
    shader.setVec3("uCameraPosition", camera.position);
    shader.setVec3("uCameraForward", camera.forward);
    shader.setVec3("uCameraRight", camera.right);
    shader.setVec3("uCameraUp", camera.up);
    shader.setFloat("uCameraFov", camera.fov);
    shader.setInt("uFrame", frameCount);
    shader.setInt("uSphereCount", scene.spheres.size());
    shader.setInt("uBoxCount", scene.boxes.size());
    shader.setVec3("uSkyColor", scene.skyColor);
    shader.setVec3("uSkyHorizon", scene.skyHorizon);
    shader.setFloat("uSkyIntensity", scene.skyIntensity);
    shader.setInt("uPlaneCount", scene.planes.size());
    shader.setInt("uTriangleCount", scene.triangles.size());


    glEnable(GL_BLEND);

    glBlendEquation(GL_FUNC_ADD);

    glBlendFunc(GL_ONE,GL_ONE);

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES,0,3);

    glDisable(GL_BLEND);

    frameCount++;

    accumulation.nextSample();

    glBindFramebuffer(GL_FRAMEBUFFER,presentFBO);

    glViewport(0,0,viewportWidth, viewportHeight);

    presentShader.use();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, accumulation.texture());

    presentShader.setInt("uAccum",0);

    presentShader.setInt("uSampleCount", accumulation.getSampleCount());

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES,0,3);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

//  Material properties layout:
//  x = material type
//  y = roughness
//  z = IOR
//  w = padding

struct gpuSphere {
    glm::vec4 positionRadius;
    glm::vec4 albedoEmission;
    glm::vec4 materialProperties;
};

struct gpuBox {
    glm::vec4 min;
    glm::vec4 max;
    glm::vec4 albedoEmission;
    glm::vec4 materialProperties;
};

struct gpuPlane {
    glm::vec4 point;
    glm::vec4 normal;
    glm::vec4 albedoEmission;
    glm::vec4 materialProperties;
};

void Renderer::sendToGPU(Scene &scene) {
    std::vector<gpuSphere> gpuSpheres;

    for (Sphere& sphere : scene.spheres) {
        gpuSphere data;

        data.positionRadius = glm::vec4(sphere.position, sphere.radius);
        data.albedoEmission = glm::vec4(sphere.material.albedo, sphere.material.emission);
        data.materialProperties = glm::vec4(sphere.material.type, sphere.material.roughness, sphere.material.ior, 0.0f);

        gpuSpheres.push_back(data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER,sphereUBO);
    glBufferData(GL_UNIFORM_BUFFER, gpuSpheres.size() * sizeof(gpuSphere), gpuSpheres.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,sphereUBO);
    glBindBuffer(GL_UNIFORM_BUFFER,0);

    //Boxes --- we do some pre-calculations here to save GPU processing --- 4th element of the vec4s is just padding

    std::vector<gpuBox> gpuBoxes;

    for (Box box : scene.boxes) {
        gpuBox data;

        glm::vec3 halfSize = box.size * 0.5f;

        data.min = glm::vec4(box.position - halfSize, 0.0f);
        data.max = glm::vec4(box.position + halfSize, 0.0f);

        data.albedoEmission = glm::vec4(box.material.albedo, box.material.emission);
        data.materialProperties = glm::vec4(box.material.type, box.material.roughness, box.material.ior, 0.0f);

        gpuBoxes.push_back(data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER,boxUBO);
    glBufferData(GL_UNIFORM_BUFFER, gpuBoxes.size() * sizeof(gpuBox), gpuBoxes.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,boxUBO);
    glBindBuffer(GL_UNIFORM_BUFFER,0);

    //Planes

    std::vector<gpuPlane> gpuPlanes;

    for (Plane plane : scene.planes) {
        gpuPlane data;

        data.point = glm::vec4(plane.point, 0.0f);
        data.normal = glm::vec4(plane.normal, 0.0f);

        data.albedoEmission = glm::vec4(plane.material.albedo, plane.material.emission);
        data.materialProperties = glm::vec4(plane.material.type, plane.material.roughness, plane.material.ior, 0.0f);

        gpuPlanes.push_back(data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER,planeUBO);
    glBufferData(GL_UNIFORM_BUFFER, gpuPlanes.size() * sizeof(gpuPlane), gpuPlanes.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,2,planeUBO);
    glBindBuffer(GL_UNIFORM_BUFFER,0);

    //Triangles
    triangleBuffer.upload(scene.triangles);
    triangleBuffer.bind(3);
}

GLuint Renderer::getRenderTexture() {
    return presentTexture;
}

void Renderer::setViewportSize(int width, int height) {
    viewportWidth = width;
    viewportHeight = height;

    accumulation.resize(width, height);

    createPresentBuffer(width, height);
}

void Renderer::resetAccumulation() {
    accumulation.reset();
    frameCount = 0;
}

void Renderer::createPresentBuffer(int width, int height) {
    if (presentWidth == width && presentHeight == height) {
        return;
    }

    if (presentFBO != 0)
    {
        glDeleteFramebuffers(1, &presentFBO);
        glDeleteTextures(1, &presentTexture);
    }

    glGenTextures(1, &presentTexture);

    glBindTexture(GL_TEXTURE_2D, presentTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &presentFBO);

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        presentFBO
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        presentTexture,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Present framebuffer is incomplete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    presentWidth = width;
    presentHeight = height;
}

Renderer::~Renderer() {
    glDeleteProgram(shader.id);
    glDeleteProgram(presentShader.id);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &sphereUBO);
    glDeleteBuffers(1, &boxUBO);
    glDeleteBuffers(1, &planeUBO);

    glDeleteFramebuffers(1, &presentFBO);
    glDeleteTextures(1, &presentTexture);
}