#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Minecraft {

class Shader {
public:
    Shader() = default;
    ~Shader();
    
    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void Bind() const;
    void Unbind() const;
    
    // Uniform setters
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    GLuint GetProgramID() const { return m_ProgramID; }

private:
    GLuint CompileShader(GLenum type, const std::string& source);
    GLint GetUniformLocation(const std::string& name);
    
    GLuint m_ProgramID = 0;
    std::unordered_map<std::string, GLint> m_UniformLocationCache;
};

} // namespace Minecraft
