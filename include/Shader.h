#pragma once
#include <string>
#include "glm/glm.hpp"

class Shader {
public:
    unsigned int id;

    Shader(const std::string& vertex_path, const std::string& fragment_path);

    void use() const;

    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setFloat(const std::string& name, float value) const;
private:
    static std::string read_file(const std::string& path);
};