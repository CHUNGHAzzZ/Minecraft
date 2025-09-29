#include "UI.h"
#include "../utils/Logger.h"
#include <sstream>
#include <iomanip>

namespace Minecraft {

// UIElement 实现
UIElement::UIElement()
    : m_position(0.0f)
    , m_size(100.0f, 20.0f)
    , m_visible(true)
{
}

// UIText 实现
UIText::UIText()
    : m_fontSize(16.0f)
    , m_color(1.0f, 1.0f, 1.0f)
{
    Logger::Debug("UIText constructor called");
}

UIText::~UIText() {
    Logger::Debug("UIText destructor called");
}

void UIText::SetText(const std::string& text) {
    m_text = text;
}

void UIText::Render() {
    if (!m_visible || m_text.empty()) {
        return;
    }

    // TODO: 实现文本渲染
    // 这里需要实现字体渲染系统
    // 暂时只记录日志
    Logger::Debug("Rendering text: " + m_text);
}

// DebugInfo 实现
DebugInfo::DebugInfo()
    : m_fps(0.0f)
    , m_playerPosition(0.0f)
    , m_chunkX(0)
    , m_chunkZ(0)
    , m_memoryUsage(0)
    , m_initialized(false)
{
    Logger::Debug("DebugInfo constructor called");
}

DebugInfo::~DebugInfo() {
    for (auto* element : m_textElements) {
        delete element;
    }
    Logger::Debug("DebugInfo destructor called");
}

void DebugInfo::SetFPS(float fps) {
    m_fps = fps;
}

void DebugInfo::SetPosition(const glm::vec3& position) {
    m_playerPosition = position;
}

void DebugInfo::SetChunkInfo(int chunkX, int chunkZ) {
    m_chunkX = chunkX;
    m_chunkZ = chunkZ;
}

void DebugInfo::SetMemoryUsage(size_t memoryMB) {
    m_memoryUsage = memoryMB;
}

void DebugInfo::Render() {
    if (!m_visible) {
        return;
    }

    // TODO: 实现调试信息渲染
    // 这里需要渲染FPS、位置、区块信息等
    Logger::Debug("Rendering debug info - FPS: " + std::to_string(m_fps) + 
                  ", Position: (" + std::to_string(m_playerPosition.x) + ", " + 
                  std::to_string(m_playerPosition.y) + ", " + 
                  std::to_string(m_playerPosition.z) + ")");
}

void DebugInfo::Update(float deltaTime) {
    // 更新调试信息显示
}

// Crosshair 实现
Crosshair::Crosshair()
    : m_color(1.0f, 1.0f, 1.0f)
    , m_size(20.0f)
{
    Logger::Debug("Crosshair constructor called");
}

Crosshair::~Crosshair() {
    Logger::Debug("Crosshair destructor called");
}

void Crosshair::Render() {
    if (!m_visible) {
        return;
    }

    // TODO: 实现十字准星渲染
    // 这里需要绘制简单的十字线
    Logger::Debug("Rendering crosshair");
}

// UIManager 实现
UIManager::UIManager()
    : m_debugInfo(nullptr)
    , m_crosshair(nullptr)
    , m_initialized(false)
{
    Logger::Info("UIManager constructor called");
}

UIManager::~UIManager() {
    Clear();
    Logger::Info("UIManager destructor called");
}

bool UIManager::Initialize() {
    Logger::Info("Initializing UI manager...");

    // 创建调试信息
    m_debugInfo = new DebugInfo();
    m_debugInfo->SetPosition(glm::vec3(10.0f, 10.0f, 0.0f));
    m_debugInfo->SetSize(glm::vec2(300.0f, 200.0f));
    AddElement(m_debugInfo);

    // 创建十字准星
    m_crosshair = new Crosshair();
    m_crosshair->SetPosition(glm::vec2(0.0f, 0.0f)); // 屏幕中心
    m_crosshair->SetSize(glm::vec2(20.0f, 20.0f));
    AddElement(m_crosshair);

    m_initialized = true;
    Logger::Info("UI manager initialized successfully");
    return true;
}

void UIManager::Render() {
    if (!m_initialized) {
        return;
    }

    // 渲染所有UI元素
    for (auto* element : m_elements) {
        if (element && element->IsVisible()) {
            element->Render();
        }
    }
}

void UIManager::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }

    // 更新所有UI元素
    for (auto* element : m_elements) {
        if (element) {
            element->Update(deltaTime);
        }
    }
}

void UIManager::AddElement(UIElement* element) {
    if (element) {
        m_elements.push_back(element);
        Logger::Debug("UI element added");
    }
}

void UIManager::RemoveElement(UIElement* element) {
    auto it = std::find(m_elements.begin(), m_elements.end(), element);
    if (it != m_elements.end()) {
        m_elements.erase(it);
        Logger::Debug("UI element removed");
    }
}

void UIManager::Clear() {
    for (auto* element : m_elements) {
        delete element;
    }
    m_elements.clear();
    
    m_debugInfo = nullptr;
    m_crosshair = nullptr;
    m_initialized = false;
    
    Logger::Debug("UI manager cleared");
}

} // namespace MinecraftFramework
