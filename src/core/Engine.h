#pragma once

#include <memory>
#include <chrono>

namespace Minecraft {

class Window;
class Renderer;
class World;
class Player;

/**
 * 游戏引擎核心类
 * 负责管理整个游戏的生命周期
 */
class Engine {
public:
    Engine();
    ~Engine();

    /**
     * 初始化引擎
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 运行游戏主循环
     */
    void Run();

    /**
     * 清理资源
     */
    void Shutdown();

    /**
     * 检查是否应该退出
     */
    bool ShouldExit() const;

private:
    /**
     * 处理输入
     */
    void HandleInput();

    /**
     * 更新游戏逻辑
     * @param deltaTime 帧时间差
     */
    void Update(float deltaTime);

    /**
     * 渲染帧
     */
    void Render();

    /**
     * 计算帧率
     */
    void CalculateFPS();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<World> m_world;
    std::unique_ptr<Player> m_player;

    bool m_running;
    bool m_initialized;

    // 帧率计算
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    float m_deltaTime;
    float m_fps;
    int m_frameCount;
    std::chrono::high_resolution_clock::time_point m_fpsStartTime;
};

} // namespace Minecraft
