#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {
class Shader;
class Texture;
class Mesh;
class Camera;

/**
 * 渲染器类
 * 负责管理OpenGL渲染状态和渲染调用
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    /**
     * 初始化渲染器
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 清理资源
     */
    void Shutdown();

    /**
     * 清除屏幕
     */
    void Clear();

    /**
     * 设置清除颜色
     */
    void SetClearColor(float r, float g, float b, float a);

    /**
     * 启用/禁用深度测试
     */
    void EnableDepthTest(bool enable);

    /**
     * 启用/禁用面剔除
     */
    void EnableCullFace(bool enable);

    /**
     * 设置面剔除模式
     */
    void SetCullFace(int mode);

    /**
     * 渲染UI
     */
    void RenderUI();

    /**
     * 获取投影矩阵
     */
    glm::mat4 GetProjectionMatrix() const;

    /**
     * 获取视图矩阵
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * 设置视图矩阵
     */
    void SetViewMatrix(const glm::mat4& view);

    /**
     * 设置投影矩阵
     */
    void SetProjectionMatrix(const glm::mat4& projection);

    /**
     * 获取基础着色器
     */
    Shader* GetBasicShader() const { return m_basicShader.get(); }

    /**
     * 获取UI着色器
     */
    Shader* GetUIShader() const { return m_uiShader.get(); }

    /**
     * 渲染网格（使用基础着色器）
     * @param mesh 要渲染的网格
     * @param modelMatrix 模型矩阵
     * @param viewMatrix 视图矩阵
     * @param projectionMatrix 投影矩阵
     */
    void RenderMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
    /**
     * 初始化OpenGL状态
     */
    void InitializeOpenGLState();

    /**
     * 创建基础着色器
     */
    bool CreateBasicShaders();

    /**
     * 创建默认纹理
     */
    bool CreateDefaultTexture();

private:
    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<Shader> m_uiShader;
    std::unique_ptr<Texture> m_defaultTexture;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;

    float m_clearColor[4];
    bool m_depthTestEnabled;
    bool m_cullFaceEnabled;
    int m_cullFaceMode;

    bool m_initialized;
};

} // namespace Minecraft
