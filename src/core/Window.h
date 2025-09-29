#pragma once

#include <string>
#include "../graphics/OpenGL.h"
#include "../player/Player.h"

namespace Minecraft {

/**
 * 窗口管理类
 * 负责创建和管理OpenGL窗口
 */
class Window {
public:
    Window();
    ~Window();

    /**
     * 初始化窗口
     * @param width 窗口宽度
     * @param height 窗口高度
     * @param title 窗口标题
     * @return 是否初始化成功
     */
    bool Initialize(int width, int height, const std::string& title);

    /**
     * 清理资源
     */
    void Shutdown();

    /**
     * 检查窗口是否应该关闭
     */
    bool ShouldClose() const;

    /**
     * 交换前后缓冲区
     */
    void SwapBuffers();

    /**
     * 轮询事件
     */
    void PollEvents();

    /**
     * 设置窗口标题
     */
    void SetTitle(const std::string& title);

    /**
     * 获取窗口尺寸
     */
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    /**
     * 获取GLFW窗口句柄
     */
    GLFWwindow* GetHandle() const { return m_window; }

private:
    /**
     * 窗口大小改变回调
     */
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    /**
     * 键盘输入回调
     */
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * 鼠标移动回调
     */
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * 鼠标滚轮回调
     */
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

public:
    /**
     * 设置全局Player实例（用于回调函数访问）
     */
    static void SetGlobalPlayer(Player* player);

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    bool m_initialized;
    
    // 全局Player实例（用于回调函数）
    static Player* s_globalPlayer;
};

} // namespace Minecraft
