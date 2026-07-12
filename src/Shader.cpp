#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/gl.h>


std::string Shader::read_file(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Error when opening: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();


    std::cout << "Shader file read successfully.";
    return buffer.str();
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_code = read_file(vertex_path);
    std::string fragment_code = read_file(fragment_path);

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    //Vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);

    //Fragment shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);

    //Compiling and linking
    id = glCreateProgram();

    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);

    glLinkProgram(id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
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
