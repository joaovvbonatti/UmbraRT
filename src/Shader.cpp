#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>


std::string Shader::read_file(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Error when opening: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();


    std::cout << "Shader file read sucessfully.";
    return buffer.str();
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_code = read_file(vertex_path);
    std::string fragment_code = read_file(fragment_path);

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    //Vertex shader
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);

    //Fragment shader
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
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