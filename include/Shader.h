#pragma once
#include <string>
#include <glad/gl.h>

class Shader {
public:
    unsigned int id;

    Shader(const std::string& vertex_path, const std::string& fragment_path);

    void use() const;
private:
    std::string read_file(const std::string& path);
};