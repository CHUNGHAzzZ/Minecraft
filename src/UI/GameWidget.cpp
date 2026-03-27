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
    glDepthFunc(GL_LEQUAL);  // 使用LEQUAL而不是默认的LESS
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
    
    // Create camera (near=0.3, far=500 for better depth precision)
    m_Camera = std::make_unique<Minecraft::Camera>(70.0f, 16.0f / 9.0f, 0.3f, 500.0f);
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
    glClearColor(13.0f/255.0f, 153.0f/255.0f, 255.0f/255.0f, 1.0f);
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
    
    // Render block outline if a block is selected
    if (m_BlockSelected) {
        RenderBlockOutline();
    }
    
    // Render crosshair (2D overlay)
    RenderCrosshair();
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
    
    // 更新方块选择
    UpdateBlockSelection();
    
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

void GameWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 破坏方块
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
    
    // 执行射线检测
    Minecraft::RaycastResult result = Minecraft::PerformRaycast(
        m_Camera->GetPosition(),
        m_Camera->GetFront(),
        m_World.get(),
        5.0f  // 最大距离5个方块
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

void GameWidget::RenderCrosshair() {
    // 保存当前OpenGL状态
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    // 设置2D正交投影
    int w = width();
    int h = height();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);  // 左上角为(0,0)
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // 计算屏幕中心
    float centerX = w / 2.0f;
    float centerY = h / 2.0f;
    
    // 准星参数
    float crosshairSize = 10.0f;  // 准星长度
    float crosshairThickness = 2.0f;  // 准星粗细
    
    // 设置红色
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // 绘制准星（两条线）
    glLineWidth(crosshairThickness);
    glBegin(GL_LINES);
    
    // 水平线
    glVertex2f(centerX - crosshairSize, centerY);
    glVertex2f(centerX + crosshairSize, centerY);
    
    // 垂直线
    glVertex2f(centerX, centerY - crosshairSize);
    glVertex2f(centerX, centerY + crosshairSize);
    
    glEnd();
    
    // 恢复OpenGL状态
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void GameWidget::RenderBlockOutline() {
    if (!m_Camera) return;
    
    // 禁用深度写入，但保持深度测试
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    
    // 启用线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    
    // 设置黑色描边
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // 方块位置（稍微扩大一点避免Z-fighting）
    float x = m_SelectedBlockX - 0.001f;
    float y = m_SelectedBlockY - 0.001f;
    float z = m_SelectedBlockZ - 0.001f;
    float size = 1.002f;
    
    // 使用固定管线绘制立方体
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(&m_Camera->GetProjectionMatrix()[0][0]);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(&m_Camera->GetViewMatrix()[0][0]);
    
    // 绘制立方体的12条边
    glBegin(GL_LINES);
    
    // 底面4条边
    glVertex3f(x, y, z);
    glVertex3f(x + size, y, z);
    
    glVertex3f(x + size, y, z);
    glVertex3f(x + size, y, z + size);
    
    glVertex3f(x + size, y, z + size);
    glVertex3f(x, y, z + size);
    
    glVertex3f(x, y, z + size);
    glVertex3f(x, y, z);
    
    // 顶面4条边
    glVertex3f(x, y + size, z);
    glVertex3f(x + size, y + size, z);
    
    glVertex3f(x + size, y + size, z);
    glVertex3f(x + size, y + size, z + size);
    
    glVertex3f(x + size, y + size, z + size);
    glVertex3f(x, y + size, z + size);
    
    glVertex3f(x, y + size, z + size);
    glVertex3f(x, y + size, z);
    
    // 4条竖边
    glVertex3f(x, y, z);
    glVertex3f(x, y + size, z);
    
    glVertex3f(x + size, y, z);
    glVertex3f(x + size, y + size, z);
    
    glVertex3f(x + size, y, z + size);
    glVertex3f(x + size, y + size, z + size);
    
    glVertex3f(x, y, z + size);
    glVertex3f(x, y + size, z + size);
    
    glEnd();
    
    // 恢复OpenGL状态
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}
