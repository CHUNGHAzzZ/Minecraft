#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <memory>

namespace Minecraft {
class Shader;
class Camera;
class World;
class Texture;
class Player;
class Inventory;
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
    void wheelEvent(QWheelEvent* event) override;

private:
    void UpdateGame();
    void SetupTimer();
    void RenderCrosshair(QPainter& painter);
    void RenderSun(QPainter& painter);
    void RenderHotbar(QPainter& painter);
    void HandleHotbarKeyInput(int key);
    void UpdateDayNight(float deltaTime);
    glm::vec3 GetSunDirection() const;
    void RenderBlockOutline();  // Legacy outline renderer (unused)
    void UpdateBlockSelection();

    std::unique_ptr<Minecraft::Shader> m_Shader;
    std::unique_ptr<Minecraft::Camera> m_Camera;
    std::unique_ptr<Minecraft::Texture> m_BlockTexture;
    std::unique_ptr<Minecraft::World> m_World;
    std::unique_ptr<Minecraft::Player> m_Player;
    std::unique_ptr<Minecraft::Inventory> m_Inventory;

    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;

    // Day-night cycle
    float m_TimeOfDay = 0.35f;              // [0,1): 0.5 is noon, 0.0/1.0 is midnight
    float m_DayLengthSeconds = 300.0f;      // Full cycle length in real-time seconds
    float m_GlobalLight = 1.0f;             // Global world light multiplier
    glm::vec3 m_SkyColor = glm::vec3(0.2f, 0.65f, 1.0f);
    QImage m_SunImage;

    bool m_BlockSelected = false;
    int m_SelectedBlockX = 0;
    int m_SelectedBlockY = 0;
    int m_SelectedBlockZ = 0;
};
