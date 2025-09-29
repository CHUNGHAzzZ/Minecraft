#include "Window.h"
#include "../utils/Logger.h"
#include <iostream>

namespace Minecraft {

// 静态成员变量定义
Player* Window::s_globalPlayer = nullptr;

Window::Window() 
    : m_window(nullptr)
    , m_width(0)
    , m_height(0)
    , m_initialized(false)
{
    Logger::Info("Window constructor called");
}

Window::~Window() {
    Shutdown();
    Logger::Info("Window destructor called");
}

bool Window::Initialize(int width, int height, const std::string& title) {
    Logger::Info("Initializing window: " + std::to_string(width) + "x" + std::to_string(height));

    // 初始化GLFW
    if (!glfwInit()) {
        Logger::Error("Failed to initialize GLFW");
        return false;
    }

    // 设置OpenGL版本和配置文件
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // 创建窗口
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        Logger::Error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    // 设置当前上下文
    glfwMakeContextCurrent(m_window);

    // 设置回调函数
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetCursorPosCallback(m_window, MouseCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);

    // 启用垂直同步
    glfwSwapInterval(1);

    // 隐藏光标并捕获
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_width = width;
    m_height = height;
    m_title = title;
    m_initialized = true;

    Logger::Info("Window initialized successfully");
    return true;
}

void Window::Shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    
    glfwTerminate();
    m_initialized = false;
    
    Logger::Info("Window shutdown complete");
}

bool Window::ShouldClose() const {
    return m_window ? glfwWindowShouldClose(m_window) : true;
}

void Window::SwapBuffers() {
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SetTitle(const std::string& title) {
    m_title = title;
    if (m_window) {
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        win->m_width = width;
        win->m_height = height;
        glViewport(0, 0, width, height);
        Logger::Info("Window resized to: " + std::to_string(width) + "x" + std::to_string(height));
    }
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        // 处理ESC键退出
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

void Window::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && s_globalPlayer) {
        // 将鼠标移动传递给Player的Input系统
        s_globalPlayer->GetInput().SetMousePositionCallback(window, xpos, ypos);
    }
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && s_globalPlayer) {
        // 将鼠标滚轮传递给Player的Input系统
        s_globalPlayer->GetInput().SetMouseScrollCallback(window, xoffset, yoffset);
    }
}

void Window::SetGlobalPlayer(Player* player) {
    s_globalPlayer = player;
}

} // namespace Minecraft
