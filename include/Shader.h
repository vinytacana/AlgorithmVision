#ifndef SHADER_H
#define SHADER_H

#include "GLLoader.h"
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID = 0;
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    void use();

    // Cache-based uniform setters
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setInt(const std::string& name, int value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec3(const std::string& name, float x, float y, float z);

private:
    std::unordered_map<std::string, int> uniformLocationCache;
    int getUniformLocation(const std::string& name);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif
