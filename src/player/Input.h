#pragma once

#include <glm/glm.hpp>
#include "../graphics/OpenGL.h"
#include <unordered_map>

namespace Minecraft {

/**
 * 输入处理类
 * 管理键盘和鼠标输入
 */
class Input {
public:
    Input();
    ~Input();

    /**
     * 初始化输入系统
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 更新输入状态
     */
    void Update();

    /**
     * 检查按键是否被按下
     * @param key GLFW按键码
     * @return 是否被按下
     */
    bool IsKeyPressed(int key) const;

    /**
     * 检查按键是否刚被按下
     * @param key GLFW按键码
     * @return 是否刚被按下
     */
    bool IsKeyJustPressed(int key) const;

    /**
     * 检查按键是否刚被释放
     * @param key GLFW按键码
     * @return 是否刚被释放
     */
    bool IsKeyJustReleased(int key) const;

    /**
     * 获取鼠标位置
     */
    glm::vec2 GetMousePosition() const { return m_mousePosition; }

    /**
     * 获取鼠标移动增量
     */
    glm::vec2 GetMouseDelta() const { return m_mouseDelta; }

    /**
     * 设置鼠标位置
     */
    void SetMousePosition(const glm::vec2& position);

    /**
     * 获取鼠标滚轮增量
     */
    float GetMouseScrollDelta() const { return m_mouseScrollDelta; }

    /**
     * 设置鼠标捕获
     */
    void SetMouseCaptured(bool captured);

    /**
     * 检查鼠标是否被捕获
     */
    bool IsMouseCaptured() const { return m_mouseCaptured; }

    /**
     * 设置按键回调
     */
    void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * 设置鼠标位置回调
     */
    void SetMousePositionCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * 设置鼠标滚轮回调
     */
    void SetMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    /**
     * 更新按键状态
     */
    void UpdateKeyStates();

    /**
     * 重置帧增量
     */
    void ResetFrameDeltas();

private:
    std::unordered_map<int, bool> m_keyStates;
    std::unordered_map<int, bool> m_previousKeyStates;
    std::unordered_map<int, bool> m_keyJustPressed;
    std::unordered_map<int, bool> m_keyJustReleased;

    glm::vec2 m_mousePosition;
    glm::vec2 m_previousMousePosition;
    glm::vec2 m_mouseDelta;
    float m_mouseScrollDelta;

    bool m_mouseCaptured;
    bool m_initialized;
};

} // namespace Minecraft
