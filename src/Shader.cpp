#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/gl.h>

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Error when opening: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::cout << "Shader file read successfully." << std::endl;
    return buffer.str();
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertexCode = readFile(vertex_path);
    std::string fragmentCode = readFile(fragment_path);

    const char* vertex_source = vertexCode.c_str();
    const char* fragment_source = fragmentCode.c_str();

    //Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_source, nullptr);
    glCompileShader(vertexShader);

    //Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_source, nullptr);
    glCompileShader(fragmentShader);

    GLint success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength);

        glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());

        std::cerr << log.data();
    }

    //Compiling and linking
    id = glCreateProgram();

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);

    glLinkProgram(id);

    GLuint sphereBlock = glGetUniformBlockIndex(id, "SphereBuffer");
    GLuint boxBlock    = glGetUniformBlockIndex(id, "BoxBuffer");

    glUniformBlockBinding(id, sphereBlock, 0);
    glUniformBlockBinding(id, boxBlock, 1);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

void Shader::setFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
