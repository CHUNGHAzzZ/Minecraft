#include "GameWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>

#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Core/Player.h"
#include "../Render/Shader.h"
#include "../Render/Camera.h"
#include "../Render/Texture.h"
#include "../World/Block.h"
#include "../World/World.h"
#include "../Utils/Logger.h"

GameWidget::GameWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    SetupTimer();
}

GameWidget::~GameWidget() {
    // 确保 OpenGL 资源在正确的上下文中释放
    makeCurrent();
    m_Player.reset();
    m_World.reset();
    m_BlockTexture.reset();
    m_Shader.reset();
    m_Camera.reset();
    doneCurrent();
}

void GameWidget::SetupTimer() {
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        Minecraft::Time::Update();
        Minecraft::Input::Update();
        UpdateGame();
        update();
    });
    timer->start(16); // ~60 FPS
}

void GameWidget::initializeGL() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        LOG_FATAL("Failed to initialize GLEW");
        qFatal("Failed to initialize GLEW");
    }
    
    LOG_INFO("OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));
    LOG_INFO("GLSL Version: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
    LOG_INFO("Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
    
    // Initialize systems
    Minecraft::Time::Init();
    Minecraft::Input::Init();
    Minecraft::Block::InitializeBlockRegistry();
    
    LOG_INFO("Game systems initialized");
    
    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Load shader
    m_Shader = std::make_unique<Minecraft::Shader>();
    if (!m_Shader->LoadFromFile("Resource/Shader/basic.vert", 
                                 "Resource/Shader/basic.frag")) {
        LOG_ERROR("Failed to load shaders");
        qWarning("Failed to load shaders");
    }
    
    // Load block texture atlas
    m_BlockTexture = std::make_unique<Minecraft::Texture>();
    if (!m_BlockTexture->LoadFromFile("Resource/Texture/default_texture.png", false)) {
        LOG_ERROR("Failed to load block texture atlas");
    } else {
        LOG_INFO("Block texture atlas loaded: " + 
                 std::to_string(m_BlockTexture->GetWidth()) + "x" + 
                 std::to_string(m_BlockTexture->GetHeight()));
    }
    
    // Create camera
    m_Camera = std::make_unique<Minecraft::Camera>(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 80.0f, 0.0f));
    LOG_INFO("Camera initialized");
    
    // Create player
    m_Player = std::make_unique<Minecraft::Player>();
    m_Player->SetPosition(glm::vec3(0.0f, 80.0f, 0.0f));
    LOG_INFO("Player created");
    
    // Create world and initialize around player
    m_World = std::make_unique<Minecraft::World>();
    m_World->SetRenderDistance(4);  // 4 chunks = 64 blocks radius
    m_World->Initialize(m_Player->GetPosition());
    LOG_INFO("World initialized with render distance: " + std::to_string(m_World->GetRenderDistance()));
    
    setCursor(Qt::BlankCursor);
    m_FirstMouse = true;
}

void GameWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    if (m_Camera) {
        m_Camera->SetAspectRatio(static_cast<float>(w) / static_cast<float>(h));
    }
}

void GameWidget::paintGL() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_Shader || !m_Camera || !m_World) return;
    
    m_Shader->Bind();
    m_Shader->SetMat4("uViewProjection", m_Camera->GetViewProjectionMatrix());
    
    // Bind block texture
    if (m_BlockTexture) {
        m_BlockTexture->Bind(0);
        m_Shader->SetInt("uTexture", 0);
    }
    
    // Render world
    m_World->Render();
    
    m_Shader->Unbind();
}

void GameWidget::UpdateGame() {
    if (!m_Camera || !m_World || !m_Player) return;
    
    float deltaTime = Minecraft::Time::DeltaTime();
    
    // 处理移动输入
    glm::vec3 moveDir(0);
    if (Minecraft::Input::IsKeyPressed(Qt::Key_W)) {
        moveDir += m_Camera->GetFront();
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_S)) {
        moveDir -= m_Camera->GetFront();
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_A)) {
        moveDir -= m_Camera->GetRight();
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_D)) {
        moveDir += m_Camera->GetRight();
    }
    
    // 水平移动（忽略Y分量）
    if (!m_Player->IsFlying()) {
        moveDir.y = 0;
    }
    
    if (glm::length(moveDir) > 0) {
        m_Player->Move(moveDir, deltaTime, m_World.get());
    }
    
    // 跳跃
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_Space)) {
        if (m_Player->IsFlying()) {
            // 飞行模式：向上移动
            m_Player->Move(glm::vec3(0, 1, 0), deltaTime, m_World.get());
        } else {
            m_Player->Jump();
        }
    }
    
    // 下降（飞行模式）
    if (Minecraft::Input::IsKeyPressed(Qt::Key_Shift) && m_Player->IsFlying()) {
        m_Player->Move(glm::vec3(0, -1, 0), deltaTime, m_World.get());
    }
    
    // 切换飞行模式
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_F)) {
        m_Player->ToggleFly();
        LOG_INFO(m_Player->IsFlying() ? "Flying mode enabled" : "Flying mode disabled");
    }
    
    // 更新玩家物理
    m_Player->Update(deltaTime, m_World.get());
    
    // 同步摄像机位置（眼睛高度1.6米）
    glm::vec3 eyePos = m_Player->GetPosition() + glm::vec3(0, 1.6f, 0);
    m_Camera->SetPosition(eyePos);
    
    // 更新世界（动态加载/卸载chunks）
    m_World->Update(m_Player->GetPosition());
    
    // ESC to exit
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_Escape)) {
        LOG_INFO("User requested exit");
        close();
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    Minecraft::Input::OnKeyPress(static_cast<Qt::Key>(event->key()));
}

void GameWidget::keyReleaseEvent(QKeyEvent* event) {
    Minecraft::Input::OnKeyRelease(static_cast<Qt::Key>(event->key()));
}

void GameWidget::mouseMoveEvent(QMouseEvent* event) {
    float xpos = event->pos().x();
    float ypos = event->pos().y();
    
    if (m_FirstMouse) {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }
    
    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos;
    m_LastX = xpos;
    m_LastY = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    if (m_Camera) {
        m_Camera->Rotate(xoffset, yoffset);
    }
    
    // Recenter cursor
    QPoint center(width() / 2, height() / 2);
    if (abs(xpos - center.x()) > 50 || abs(ypos - center.y()) > 50) {
        cursor().setPos(mapToGlobal(center));
        m_LastX = center.x();
        m_LastY = center.y();
    }
}
