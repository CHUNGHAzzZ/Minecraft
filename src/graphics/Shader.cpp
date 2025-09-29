#include "Shader.h"
#include "../utils/Logger.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Minecraft {

Shader::Shader()
    : m_programID(0)
    , m_initialized(false)
{
    Logger::Info("Shader constructor called");
}

Shader::~Shader() {
    Cleanup();
    Logger::Info("Shader destructor called");
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    Logger::Info("Loading shader from source...");

    // 编译顶点着色器
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        Logger::Error("Failed to compile vertex shader");
        return false;
    }

    // 编译片段着色器
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        Logger::Error("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        return false;
    }

    // 链接程序
    if (!LinkProgram(vertexShader, fragmentShader)) {
        Logger::Error("Failed to link shader program");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // 清理着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_initialized = true;
    Logger::Info("Shader loaded successfully");
    return true;
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    Logger::Info("Loading shader from files: " + vertexPath + ", " + fragmentPath);

    // 读取顶点着色器文件
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        Logger::Error("Failed to open vertex shader file: " + vertexPath);
        return false;
    }

    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexFile.close();

    // 读取片段着色器文件
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        Logger::Error("Failed to open fragment shader file: " + fragmentPath);
        return false;
    }

    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentFile.close();

    return LoadFromSource(vertexStream.str(), fragmentStream.str());
}

void Shader::Use() const {
    if (m_programID != 0) {
        glUseProgram(m_programID);
    }
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(m_programID, name.c_str()), x, y, z);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::CompileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    if (!CheckCompileErrors(shader, (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT")) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::LinkProgram(GLuint vertex, GLuint fragment) {
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);

    return CheckLinkErrors();
}

bool Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        Logger::Error("Shader compilation error (" + type + "): " + std::string(infoLog));
        return false;
    }

    return true;
}

bool Shader::CheckLinkErrors() {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(m_programID, 1024, nullptr, infoLog);
        Logger::Error("Shader linking error: " + std::string(infoLog));
        return false;
    }

    return true;
}

void Shader::Cleanup() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
    m_initialized = false;
}

} // namespace Minecraft
