#include "core/Engine.h"
#include "utils/Logger.h"
#include <iostream>
#include <exception>

using namespace Minecraft;

int main() {
    try {
        // 初始化日志系统
        Logger::Initialize("minecraft_framework.log");
        Logger::Info("Starting Minecraft Framework...");

        // 创建并初始化引擎
        Engine engine;
        if (!engine.Initialize()) {
            Logger::Error("Failed to initialize engine");
            return -1;
        }

        // 运行游戏
        engine.Run();

        Logger::Info("Minecraft Framework shutdown");
        Logger::Shutdown();
        return 0;

    } catch (const std::exception& e) {
        Logger::Error("Unhandled exception: " + std::string(e.what()));
        Logger::Shutdown();
        return -1;
    } catch (...) {
        Logger::Error("Unknown exception occurred");
        Logger::Shutdown();
        return -1;
    }
}
