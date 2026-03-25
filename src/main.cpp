#include <QApplication>
#include <QSurfaceFormat>
#include "UI/GameWidget.h"
#include "Utils/Logger.h"

int main(int argc, char** argv) {
    // Initialize logger
    Minecraft::Logger::Init("minecraft.log", true);
    LOG_INFO("========== Minecraft Clone Starting ==========");
    
    // Setup OpenGL format
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);
    
    LOG_INFO("OpenGL format configured: 3.3 Core Profile");

    QApplication app(argc, argv);

    GameWidget window;
    window.resize(1280, 720);
    window.setWindowTitle("Minecraft Clone");
    window.show();
    
    LOG_INFO("Application window created and shown");

    int result = app.exec();
    
    LOG_INFO("Application exiting with code: " + std::to_string(result));
    Minecraft::Logger::Shutdown();
    
    return result;
}
