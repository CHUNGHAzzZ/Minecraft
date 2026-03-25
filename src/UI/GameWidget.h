#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QTimer>
#include <memory>
#include <vector>

namespace Minecraft {
    class Shader;
    class Camera;
    class Chunk;
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
    std::vector<std::unique_ptr<Minecraft::Chunk>> m_Chunks;
    
    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
};
