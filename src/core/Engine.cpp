#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include "../world/World.h"
#include "../player/Player.h"
#include "../utils/Logger.h"
#include <iostream>

namespace Minecraft {

Engine::Engine() 
    : m_running(false)
    , m_initialized(false)
    , m_deltaTime(0.0f)
    , m_fps(0.0f)
    , m_frameCount(0)
{
    Logger::Info("Engine constructor called");
}

Engine::~Engine() {
    Shutdown();
    Logger::Info("Engine destructor called");
}

bool Engine::Initialize() {
    Logger::Info("Initializing engine...");

    // 创建窗口
    m_window = std::make_unique<Window>();
    if (!m_window->Initialize(1280, 720, "Minecraft Framework")) {
        Logger::Error("Failed to initialize window");
        return false;
    }

    // 创建渲染器
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Initialize()) {
        Logger::Error("Failed to initialize renderer");
        return false;
    }

    // 创建世界
    m_world = std::make_unique<World>();
    if (!m_world->Initialize()) {
        Logger::Error("Failed to initialize world");
        return false;
    }

    // 创建玩家
    m_player = std::make_unique<Player>();
    if (!m_player->Initialize()) {
        Logger::Error("Failed to initialize player");
        return false;
    }

    m_initialized = true;
    m_running = true;
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_fpsStartTime = m_lastFrameTime;

    Logger::Info("Engine initialized successfully");
    return true;
}

void Engine::Run() {
    if (!m_initialized) {
        Logger::Error("Engine not initialized");
        return;
    }

    Logger::Info("Starting game loop...");

    while (m_running && !m_window->ShouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        m_deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
        m_lastFrameTime = currentTime;

        HandleInput();
        Update(m_deltaTime);
        Render();
        CalculateFPS();

        m_window->SwapBuffers();
        m_window->PollEvents();
    }

    Logger::Info("Game loop ended");
}

void Engine::Shutdown() {
    if (m_player) {
        m_player.reset();
    }
    
    if (m_world) {
        m_world.reset();
    }
    
    if (m_renderer) {
        m_renderer.reset();
    }
    
    if (m_window) {
        m_window.reset();
    }

    m_initialized = false;
    m_running = false;
    
    Logger::Info("Engine shutdown complete");
}

bool Engine::ShouldExit() const {
    return !m_running || m_window->ShouldClose();
}

void Engine::HandleInput() {
    // 处理输入事件
    if (m_player) {
        m_player->HandleInput(m_deltaTime);
    }
}

void Engine::Update(float deltaTime) {
    // 更新游戏逻辑
    if (m_player) {
        m_player->Update(deltaTime);
    }
    
    if (m_world) {
        m_world->Update(deltaTime);
    }
}

void Engine::Render() {
    // 清除屏幕
    m_renderer->Clear();

    // 渲染世界
    if (m_world && m_player) {
        m_world->Render(*m_renderer, m_player->GetCamera());
    }

    // 渲染UI
    m_renderer->RenderUI();
}

void Engine::CalculateFPS() {
    m_frameCount++;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<float>(currentTime - m_fpsStartTime).count();
    
    if (elapsed >= 1.0f) {
        m_fps = m_frameCount / elapsed;
        m_frameCount = 0;
        m_fpsStartTime = currentTime;
        
        // 更新窗口标题显示FPS
        std::string title = "Minecraft Framework - FPS: " + std::to_string(static_cast<int>(m_fps));
        m_window->SetTitle(title);
    }
}

} // namespace Minecraft
