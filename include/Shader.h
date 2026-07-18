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
    void setInt(const std::string& name, int value) const;
private:
    static std::string readFile(const std::string& path);
};