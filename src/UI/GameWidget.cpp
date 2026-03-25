#include "GameWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>

#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Render/Shader.h"
#include "../Render/Camera.h"
#include "../World/Block.h"
#include "../World/Chunk.h"
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
    m_Chunks.clear();
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
    if (!m_Shader->LoadFromFile("src/Resource/Shader/basic.vert", 
                                 "src/Resource/Shader/basic.frag")) {
        LOG_ERROR("Failed to load shaders");
        qWarning("Failed to load shaders");
    }
    
    // Create camera
    m_Camera = std::make_unique<Minecraft::Camera>(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    m_Camera->SetPosition(glm::vec3(8.0f, 80.0f, 8.0f));
    LOG_INFO("Camera initialized at position (8, 80, 8)");
    
    // Generate test chunks
    LOG_INFO("Generating world chunks...");
    for (int x = -2; x <= 2; ++x) {
        for (int z = -2; z <= 2; ++z) {
            auto chunk = std::make_unique<Minecraft::Chunk>(x, z);
            chunk->GenerateTerrain();
            chunk->BuildMesh();
            m_Chunks.push_back(std::move(chunk));
        }
    }
    LOG_INFO("Generated " + std::to_string(m_Chunks.size()) + " chunks");
    
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
    
    if (!m_Shader || !m_Camera) return;
    
    m_Shader->Bind();
    m_Shader->SetMat4("uViewProjection", m_Camera->GetViewProjectionMatrix());
    
    for (const auto& chunk : m_Chunks) {
        chunk->Render();
    }
    
    m_Shader->Unbind();
}

void GameWidget::UpdateGame() {
    if (!m_Camera) return;
    
    float speed = 10.0f * Minecraft::Time::DeltaTime();
    
    // Movement
    if (Minecraft::Input::IsKeyPressed(Qt::Key_W)) {
        m_Camera->Move(m_Camera->GetFront() * speed);
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_S)) {
        m_Camera->Move(-m_Camera->GetFront() * speed);
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_A)) {
        m_Camera->Move(-m_Camera->GetRight() * speed);
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_D)) {
        m_Camera->Move(m_Camera->GetRight() * speed);
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_Space)) {
        m_Camera->Move(glm::vec3(0, 1, 0) * speed);
    }
    if (Minecraft::Input::IsKeyPressed(Qt::Key_Shift)) {
        m_Camera->Move(glm::vec3(0, -1, 0) * speed);
    }
    
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
