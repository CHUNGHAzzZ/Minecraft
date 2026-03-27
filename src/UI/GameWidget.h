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
    void mousePressEvent(QMouseEvent* event) override;

private:
    void UpdateGame();
    void SetupTimer();
    void RenderCrosshair();  // 绘制准星
    void RenderBlockOutline();  // 绘制方块描边
    void UpdateBlockSelection();  // 更新选中的方块
    
    std::unique_ptr<Minecraft::Shader> m_Shader;
    std::unique_ptr<Minecraft::Camera> m_Camera;
    std::unique_ptr<Minecraft::Texture> m_BlockTexture;  // 方块纹理图集
    std::unique_ptr<Minecraft::World> m_World;  // 世界管理器
    std::unique_ptr<Minecraft::Player> m_Player;  // 玩家
    
    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
    
    // 方块选择
    bool m_BlockSelected = false;  // 是否选中方块
    int m_SelectedBlockX = 0;
    int m_SelectedBlockY = 0;
    int m_SelectedBlockZ = 0;
};
