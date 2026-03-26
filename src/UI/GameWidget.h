#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QTimer>
#include <memory>
#include <vector>

namespace Minecraft {
    class Shader;
    class Camera;
    class World;
    class Texture;
    class Player;
}

class GameWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    ~GameWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void UpdateGame();
    void SetupTimer();
    
    std::unique_ptr<Minecraft::Shader> m_Shader;
    std::unique_ptr<Minecraft::Camera> m_Camera;
    std::unique_ptr<Minecraft::Texture> m_BlockTexture;  // 方块纹理图集
    std::unique_ptr<Minecraft::World> m_World;  // 世界管理器
    std::unique_ptr<Minecraft::Player> m_Player;  // 玩家
    
    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
};
