#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGL.h"

namespace Minecraft {

/**
 * 着色器管理类
 * 负责编译和管理OpenGL着色器程序
 */
class Shader {
public:
    Shader();
    ~Shader();

    /**
     * 从源码加载着色器
     * @param vertexSource 顶点着色器源码
     * @param fragmentSource 片段着色器源码
     * @return 是否加载成功
     */
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * 从文件加载着色器
     * @param vertexPath 顶点着色器文件路径
     * @param fragmentPath 片段着色器文件路径
     * @return 是否加载成功
     */
    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * 使用着色器程序
     */
    void Use() const;

    /**
     * 设置布尔值uniform
     */
    void SetBool(const std::string& name, bool value) const;

    /**
     * 设置整数值uniform
     */
    void SetInt(const std::string& name, int value) const;

    /**
     * 设置浮点值uniform
     */
    void SetFloat(const std::string& name, float value) const;

    /**
     * 设置向量uniform
     */
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;

    /**
     * 设置矩阵uniform
     */
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    /**
     * 获取程序ID
     */
    GLuint GetProgramID() const { return m_programID; }

    /**
     * 检查是否有效
     */
    bool IsValid() const { return m_programID != 0; }

private:
    /**
     * 编译着色器
     * @param source 着色器源码
     * @param type 着色器类型
     * @return 着色器ID，失败返回0
     */
    GLuint CompileShader(const std::string& source, GLenum type);

    /**
     * 链接程序
     * @param vertex 顶点着色器ID
     * @param fragment 片段着色器ID
     * @return 是否链接成功
     */
    bool LinkProgram(GLuint vertex, GLuint fragment);

    /**
     * 检查编译错误
     * @param shader 着色器ID
     * @param type 着色器类型名称
     * @return 是否编译成功
     */
    bool CheckCompileErrors(GLuint shader, const std::string& type);

    /**
     * 检查链接错误
     * @return 是否链接成功
     */
    bool CheckLinkErrors();

    /**
     * 清理资源
     */
    void Cleanup();

private:
    GLuint m_programID;
    bool m_initialized;
};

} // namespace Minecraft
