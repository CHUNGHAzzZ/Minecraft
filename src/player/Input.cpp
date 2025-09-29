#include "Input.h"
#include "../utils/Logger.h"

namespace Minecraft {

Input::Input()
    : m_mousePosition(0.0f)
    , m_previousMousePosition(0.0f)
    , m_mouseDelta(0.0f)
    , m_mouseScrollDelta(0.0f)
    , m_mouseCaptured(false)
    , m_initialized(false)
{
    Logger::Info("Input constructor called");
}

Input::~Input() {
    Logger::Info("Input destructor called");
}

bool Input::Initialize() {
    Logger::Info("Initializing input system...");
    
    m_initialized = true;
    Logger::Info("Input system initialized successfully");
    return true;
}

void Input::Update() {
    if (!m_initialized) {
        return;
    }

    UpdateKeyStates();
    ResetFrameDeltas();
}

bool Input::IsKeyPressed(int key) const {
    auto it = m_keyStates.find(key);
    return (it != m_keyStates.end()) ? it->second : false;
}

bool Input::IsKeyJustPressed(int key) const {
    auto it = m_keyJustPressed.find(key);
    return (it != m_keyJustPressed.end()) ? it->second : false;
}

bool Input::IsKeyJustReleased(int key) const {
    auto it = m_keyJustReleased.find(key);
    return (it != m_keyJustReleased.end()) ? it->second : false;
}

void Input::SetMousePosition(const glm::vec2& position) {
    m_previousMousePosition = m_mousePosition;
    m_mousePosition = position;
    m_mouseDelta = m_mousePosition - m_previousMousePosition;
}

void Input::SetMouseCaptured(bool captured) {
    m_mouseCaptured = captured;
}

void Input::SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        m_keyStates[key] = true;
        m_keyJustPressed[key] = true;
    } else if (action == GLFW_RELEASE) {
        m_keyStates[key] = false;
        m_keyJustReleased[key] = true;
    }
}

void Input::SetMousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    SetMousePosition(glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos)));
}

void Input::SetMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    m_mouseScrollDelta = static_cast<float>(yoffset);
}

void Input::UpdateKeyStates() {
    // 更新按键状态
    for (auto& pair : m_keyStates) {
        int key = pair.first;
        bool currentState = pair.second;
        bool previousState = m_previousKeyStates[key];
        
        m_keyJustPressed[key] = currentState && !previousState;
        m_keyJustReleased[key] = !currentState && previousState;
        m_previousKeyStates[key] = currentState;
    }
}

void Input::ResetFrameDeltas() {
    // 重置帧增量
    m_keyJustPressed.clear();
    m_keyJustReleased.clear();
    m_mouseDelta = glm::vec2(0.0f);
    m_mouseScrollDelta = 0.0f;
}

} // namespace Minecraft
