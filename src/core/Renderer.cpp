#include "Renderer.h"
#include "../graphics/Shader.h"
#include "../utils/Logger.h"
#include "../graphics/OpenGL.h"
#include "../graphics/Mesh.h"
#include "../graphics/Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

Renderer::Renderer()
    : m_clearColor{0.5f, 0.8f, 1.0f, 1.0f}  // 天空蓝
    , m_depthTestEnabled(true)
    , m_cullFaceEnabled(true)
    , m_cullFaceMode(GL_BACK)
    , m_initialized(false)
{
    Logger::Info("Renderer constructor called");
}

Renderer::~Renderer() {
    Shutdown();
    Logger::Info("Renderer destructor called");
}

bool Renderer::Initialize() {
    Logger::Info("Initializing renderer...");

    // 初始化GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        Logger::Error("Failed to initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
        return false;
    }

    // 初始化OpenGL状态
    InitializeOpenGLState();

    // 创建基础着色器
    if (!CreateBasicShaders()) {
        Logger::Error("Failed to create basic shaders");
        return false;
    }

    // 创建默认纹理
    if (!CreateDefaultTexture()) {
        Logger::Error("Failed to create default texture");
        return false;
    }

    // 设置默认投影矩阵
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
    m_viewMatrix = glm::mat4(1.0f);

    m_initialized = true;
    Logger::Info("Renderer initialized successfully");
    return true;
}

void Renderer::Shutdown() {
    if (m_basicShader) {
        m_basicShader.reset();
    }
    
    if (m_uiShader) {
        m_uiShader.reset();
    }

    m_initialized = false;
    Logger::Info("Renderer shutdown complete");
}

void Renderer::Clear() {
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetClearColor(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void Renderer::EnableDepthTest(bool enable) {
    m_depthTestEnabled = enable;
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::EnableCullFace(bool enable) {
    m_cullFaceEnabled = enable;
    if (enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(m_cullFaceMode);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::SetCullFace(int mode) {
    m_cullFaceMode = mode;
    if (m_cullFaceEnabled) {
        glCullFace(mode);
    }
}

void Renderer::RenderUI() {
    // TODO: 实现UI渲染
    // 这里可以渲染调试信息、HUD等
}

glm::mat4 Renderer::GetProjectionMatrix() const {
    return m_projectionMatrix;
}

glm::mat4 Renderer::GetViewMatrix() const {
    return m_viewMatrix;
}

void Renderer::SetViewMatrix(const glm::mat4& view) {
    m_viewMatrix = view;
}

void Renderer::SetProjectionMatrix(const glm::mat4& projection) {
    m_projectionMatrix = projection;
}

void Renderer::InitializeOpenGLState() {
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 启用面剔除
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 设置混合模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Logger::Info("OpenGL state initialized");
}

bool Renderer::CreateBasicShaders() {
    // 从GLSL文件加载着色器
    m_basicShader = std::make_unique<Shader>();
    if (!m_basicShader->LoadFromFile("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl")) {
        Logger::Error("Failed to create basic shader from files");
        return false;
    }

    Logger::Info("Basic shaders loaded from files successfully");
    return true;
}

bool Renderer::CreateDefaultTexture() {
    Logger::Info("Creating default texture...");
    
    m_defaultTexture = std::make_unique<Texture>();
    
    // 尝试从文件加载默认纹理
    if (m_defaultTexture->LoadFromFile("assets/textures/default_texture.png")) {
        Logger::Info("Default texture loaded from file successfully");
        return true;
    }
    
    // 如果文件加载失败，创建一个简单的程序化纹理
    Logger::Warning("Failed to load default texture from file, creating procedural texture");
    
    // 创建一个简单的白色纹理
    unsigned char whiteTexture[4] = {255, 255, 255, 255}; // RGBA
    if (m_defaultTexture->LoadFromMemory(whiteTexture, 1, 1, 4)) {
        Logger::Info("Procedural default texture created successfully");
        return true;
    }
    
    Logger::Error("Failed to create default texture");
    return false;
}

void Renderer::RenderMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (!mesh) {
        Logger::Warning("Renderer::RenderMesh called with null mesh");
        return;
    }
    
    if (!m_basicShader) {
        Logger::Error("Renderer::RenderMesh called with null shader");
        return;
    }

    if (!mesh->HasData()) {
        Logger::Warning("Renderer::RenderMesh called with empty mesh");
        return;
    }

    Logger::Debug("Rendering mesh with " + std::to_string(mesh->GetVertexCount()) + " vertices");

    // 使用基础着色器
    m_basicShader->Use();

    // 设置MVP矩阵
    m_basicShader->SetMat4("model", modelMatrix);
    m_basicShader->SetMat4("view", viewMatrix);
    m_basicShader->SetMat4("projection", projectionMatrix);

    // 设置光照参数（使用默认值）
    m_basicShader->SetVec3("lightPos", glm::vec3(100.0f, 100.0f, 100.0f));
    m_basicShader->SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_basicShader->SetVec3("viewPos", glm::vec3(0.0f, 0.0f, 0.0f)); // 这里应该使用摄像机位置

    // 绑定默认纹理（如果存在）
    if (m_defaultTexture) {
        m_defaultTexture->Bind(0);
        m_basicShader->SetInt("texture1", 0);
    } else {
        Logger::Warning("No default texture available for rendering");
    }

    // 渲染网格
    mesh->Render();
}

} // namespace Minecraft
