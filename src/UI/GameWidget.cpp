#include "GameWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>
#include <glm/gtc/constants.hpp>

#include "../Core/Time.h"
#include "../Core/Inventory.h"
#include "../Core/Input.h"
#include "../Core/Player.h"
#include "../Render/Shader.h"
#include "../Render/Camera.h"
#include "../Render/Texture.h"
#include "../World/Block.h"
#include "../World/World.h"
#include "../World/Raycast.h"
#include "../Utils/Logger.h"

GameWidget::GameWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    SetupTimer();
}

GameWidget::~GameWidget() {
    makeCurrent();
    m_Inventory.reset();
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
        UpdateGame();
        Minecraft::Input::Update();
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
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Load shader
    m_Shader = std::make_unique<Minecraft::Shader>();
    if (!m_Shader->LoadFromFile("Resource/Shader/basic.vert", 
                                 "Resource/Shader/basic.frag")) {
        LOG_ERROR("Failed to load shaders");
        qWarning("Failed to load shaders");
    }
    
    // Load block texture atlas as texture array
    m_BlockTexture = std::make_unique<Minecraft::Texture>();
    if (!m_BlockTexture->LoadAsTextureArray("Resource/Texture/default_texture.png", 16, 16)) {
        LOG_ERROR("Failed to load block texture atlas");
    } else {
        LOG_INFO("Block texture atlas loaded: " + 
                 std::to_string(m_BlockTexture->GetWidth()) + "x" + 
                 std::to_string(m_BlockTexture->GetHeight()));
    }

    m_SunImage = QImage("Resource/Texture/sun.png");
    if (m_SunImage.isNull()) {
        LOG_WARNING("Failed to load sun texture: Resource/Texture/sun.png");
    } else {
        LOG_INFO("Sun texture loaded");
    }
    
    // Create camera (near=0.3, far=500 for better depth precision)
    m_Camera = std::make_unique<Minecraft::Camera>(70.0f, 16.0f / 9.0f, 0.3f, 500.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 80.0f, 0.0f));
    LOG_INFO("Camera initialized");
    
    // Create player
    m_Player = std::make_unique<Minecraft::Player>();
    m_Player->SetPosition(glm::vec3(0.0f, 180.0f, 0.0f));
    LOG_INFO("Player created");

    // Create inventory/hotbar
    m_Inventory = std::make_unique<Minecraft::Inventory>();
    LOG_INFO("Inventory initialized");
    
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    glClearColor(m_SkyColor.r, m_SkyColor.g, m_SkyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_Shader || !m_Camera || !m_World) return;
    
    m_Shader->Bind();
    m_Shader->SetMat4("uViewProjection", m_Camera->GetViewProjectionMatrix());
    m_Shader->SetFloat("uGlobalLight", m_GlobalLight);
    
    // Bind block texture
    if (m_BlockTexture) {
        m_BlockTexture->Bind(0);
        m_Shader->SetInt("uTexture", 0);
    }
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    m_World->RenderOpaque();
    
    m_Shader->Unbind();
    
    // Render 2D HUD overlay
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    RenderSun(painter);
    RenderCrosshair(painter);
    RenderHotbar(painter);
}

void GameWidget::UpdateGame() {
    if (!m_Camera || !m_World || !m_Player) return;
    
    float deltaTime = Minecraft::Time::DeltaTime();
    UpdateDayNight(deltaTime);
    
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
    
    if (!m_Player->IsFlying()) {
        moveDir.y = 0;
    }
    
    const bool isSprinting = !m_Player->IsFlying() && Minecraft::Input::IsKeyPressed(Qt::Key_Shift);
    if (glm::length(moveDir) > 0) {
        m_Player->Move(moveDir, deltaTime, m_World.get(), isSprinting);
    }
    
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_Space)) {
        if (m_Player->IsFlying()) {
            m_Player->Move(glm::vec3(0, 1, 0), deltaTime, m_World.get());
        } else {
            m_Player->Jump();
        }
    }
    
    if (Minecraft::Input::IsKeyPressed(Qt::Key_Shift) && m_Player->IsFlying()) {
        m_Player->Move(glm::vec3(0, -1, 0), deltaTime, m_World.get());
    }
    
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_F)) {
        m_Player->ToggleFly();
        LOG_INFO(m_Player->IsFlying() ? "Flying mode enabled" : "Flying mode disabled");
    }
    
    m_Player->Update(deltaTime, m_World.get());
    
    glm::vec3 eyePos = m_Player->GetPosition() + glm::vec3(0, 1.6f, 0);
    m_Camera->SetPosition(eyePos);

    m_World->Update(m_Player->GetPosition());
    UpdateBlockSelection();

    // ESC to exit
    if (Minecraft::Input::IsKeyJustPressed(Qt::Key_Escape)) {
        LOG_INFO("User requested exit");
        close();
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        HandleHotbarKeyInput(event->key());
    }
    Minecraft::Input::OnKeyPress(static_cast<Qt::Key>(event->key()));
}

void GameWidget::keyReleaseEvent(QKeyEvent* event) {
    Minecraft::Input::OnKeyRelease(static_cast<Qt::Key>(event->key()));
}

void GameWidget::wheelEvent(QWheelEvent* event) {
    if (!m_Inventory) {
        return;
    }

    const QPoint angleDelta = event->angleDelta();
    if (angleDelta.y() > 0) {
        m_Inventory->SelectPrevSlot();
    } else if (angleDelta.y() < 0) {
        m_Inventory->SelectNextSlot();
    }
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

void GameWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_BlockSelected && m_World) {
            if (m_World->BreakBlock(m_SelectedBlockX, m_SelectedBlockY, m_SelectedBlockZ)) {
                LOG_INFO("Block broken at (" +
                         std::to_string(m_SelectedBlockX) + ", " +
                         std::to_string(m_SelectedBlockY) + ", " +
                         std::to_string(m_SelectedBlockZ) + ")");
            }
        }
    }
}

void GameWidget::UpdateBlockSelection() {
    if (!m_Camera || !m_World) {
        m_BlockSelected = false;
        return;
    }

    Minecraft::RaycastResult result = Minecraft::PerformRaycast(
        m_Camera->GetPosition(),
        m_Camera->GetFront(),
        m_World.get(),
        5.0f
    );

    if (result.hit) {
        m_BlockSelected = true;
        m_SelectedBlockX = result.blockX;
        m_SelectedBlockY = result.blockY;
        m_SelectedBlockZ = result.blockZ;
    } else {
        m_BlockSelected = false;
    }
}

void GameWidget::RenderCrosshair(QPainter& painter) {
    painter.setPen(QPen(QColor(255, 40, 40), 2));

    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int crosshairSize = 8;

    painter.drawLine(centerX - crosshairSize, centerY, centerX + crosshairSize, centerY);
    painter.drawLine(centerX, centerY - crosshairSize, centerX, centerY + crosshairSize);
}

void GameWidget::RenderSun(QPainter& painter) {
    if (m_SunImage.isNull() || !m_Camera) {
        return;
    }

    const glm::vec3 sunDir = GetSunDirection();
    const glm::vec3 sunWorldPos = m_Camera->GetPosition() + sunDir * 220.0f;
    const glm::vec4 clip = m_Camera->GetViewProjectionMatrix() * glm::vec4(sunWorldPos, 1.0f);
    if (clip.w <= 0.001f) {
        return;
    }

    const glm::vec3 ndc = glm::vec3(clip) / clip.w;
    if (ndc.z < -1.0f || ndc.z > 1.0f) {
        return;
    }
    if (ndc.x < -1.2f || ndc.x > 1.2f || ndc.y < -1.2f || ndc.y > 1.2f) {
        return;
    }

    const int centerX = static_cast<int>((ndc.x * 0.5f + 0.5f) * static_cast<float>(width()));
    const int centerY = static_cast<int>((1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(height()));
    const int sunSize = 72;
    painter.drawImage(QRect(centerX - sunSize / 2, centerY - sunSize / 2, sunSize, sunSize), m_SunImage);
}

glm::vec3 GameWidget::GetSunDirection() const {
    const float angle = m_TimeOfDay * glm::two_pi<float>();
    return glm::normalize(glm::vec3(std::cos(angle), std::sin(angle), -0.25f));
}

void GameWidget::UpdateDayNight(float deltaTime) {
    if (m_DayLengthSeconds <= 0.0f) {
        return;
    }

    m_TimeOfDay += deltaTime / m_DayLengthSeconds;
    if (m_TimeOfDay >= 1.0f) {
        m_TimeOfDay -= std::floor(m_TimeOfDay);
    }

    const float sunHeight = GetSunDirection().y;
    const float daylight = glm::clamp((sunHeight + 0.15f) / 0.65f, 0.0f, 1.0f);
    const float twilight = glm::clamp(1.0f - std::abs(sunHeight) * 4.5f, 0.0f, 1.0f);

    m_GlobalLight = glm::mix(0.22f, 1.0f, daylight);

    const glm::vec3 nightSky(0.02f, 0.03f, 0.08f);
    const glm::vec3 daySky(0.05f, 0.60f, 0.98f);
    const glm::vec3 sunsetSky(1.0f, 0.45f, 0.20f);
    m_SkyColor = glm::mix(nightSky, daySky, daylight);
    m_SkyColor = glm::mix(m_SkyColor, sunsetSky, twilight * 0.45f);
}

void GameWidget::RenderHotbar(QPainter& painter) {
    if (!m_Inventory) {
        return;
    }

    constexpr int slotCount = Minecraft::Inventory::HOTBAR_SIZE;
    constexpr int slotSize = 52;
    constexpr int slotGap = 8;
    const int totalWidth = slotCount * slotSize + (slotCount - 1) * slotGap;
    const int startX = (width() - totalWidth) / 2;
    const int startY = height() - slotSize - 28;

    painter.setFont(QFont("Consolas", 10, QFont::Bold));

    for (int i = 0; i < slotCount; ++i) {
        const int x = startX + i * (slotSize + slotGap);
        const QRect slotRect(x, startY, slotSize, slotSize);
        const bool selected = (i == m_Inventory->GetSelectedSlot());

        painter.fillRect(slotRect, QColor(26, 26, 26, 180));
        painter.setPen(selected ? QPen(QColor(255, 230, 120), 3) : QPen(QColor(205, 205, 205), 2));
        painter.drawRect(slotRect);

        const Minecraft::ItemStack& stack = m_Inventory->GetSlot(i);
        if (!stack.IsEmpty()) {
            const Minecraft::BlockData& block = Minecraft::Block::GetBlockData(stack.type);
            painter.setPen(QColor(255, 255, 255));
            painter.drawText(slotRect.adjusted(6, 6, -6, -22), Qt::AlignLeft | Qt::AlignTop, QString::fromStdString(block.name));

            if (stack.count > 1) {
                painter.setPen(QColor(255, 255, 255));
                painter.drawText(slotRect.adjusted(0, 0, -4, -4), Qt::AlignRight | Qt::AlignBottom, QString::number(stack.count));
            }
        }

        painter.setPen(QColor(245, 245, 245, 220));
        painter.drawText(QRect(x, startY + slotSize + 3, slotSize, 16), Qt::AlignCenter, QString::number((i + 1) % 10));
    }
}

void GameWidget::HandleHotbarKeyInput(int key) {
    if (!m_Inventory) {
        return;
    }

    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
        m_Inventory->SetSelectedSlot(key - Qt::Key_1);
    }
}


