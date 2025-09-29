#include "Renderer.h"
#include "../graphics/Shader.h"
#include "../utils/Logger.h"
#include "../graphics/OpenGL.h"

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
    // 基础顶点着色器源码
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec3 aNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec2 TexCoord;
        out vec3 Normal;
        out vec3 FragPos;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    // 基础片段着色器源码
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec3 Normal;
        in vec3 FragPos;

        uniform sampler2D texture1;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;

        out vec4 FragColor;

        void main() {
            // 环境光
            float ambientStrength = 0.1;
            vec3 ambient = ambientStrength * lightColor;

            // 漫反射
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            // 镜面反射
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;

            vec3 result = (ambient + diffuse + specular);
            FragColor = texture(texture1, TexCoord) * vec4(result, 1.0);
        }
    )";

    m_basicShader = std::make_unique<Shader>();
    if (!m_basicShader->LoadFromSource(vertexShaderSource, fragmentShaderSource)) {
        Logger::Error("Failed to create basic shader");
        return false;
    }

    Logger::Info("Basic shaders created successfully");
    return true;
}

} // namespace Minecraft
