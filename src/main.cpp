#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QTimer>
#include <memory>

class GLWidget final : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    explicit GLWidget(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
    {
        setFocusPolicy(Qt::StrongFocus);

        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            m_angle += 0.5f;
            update();
        });
        timer->start(16);
    }

protected:
    void initializeGL() override
    {
        initializeOpenGLFunctions();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        static const char* kVert = R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aColor;
            uniform mat4 uMVP;
            out vec3 vColor;
            void main(){
                vColor = aColor;
                gl_Position = uMVP * vec4(aPos, 1.0);
            }
        )";

        static const char* kFrag = R"(
            #version 330 core
            in vec3 vColor;
            out vec4 FragColor;
            void main(){
                FragColor = vec4(vColor, 1.0);
            }
        )";

        m_program = std::make_unique<QOpenGLShaderProgram>();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, kVert);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, kFrag);
        m_program->link();

        // 36 vertices (12 triangles) cube
        const float v[] = {
            // positions          // colors
            -0.5f,-0.5f, 0.5f,    1.0f,0.2f,0.2f,
             0.5f,-0.5f, 0.5f,    1.0f,0.2f,0.2f,
             0.5f, 0.5f, 0.5f,    1.0f,0.2f,0.2f,
            -0.5f,-0.5f, 0.5f,    1.0f,0.2f,0.2f,
             0.5f, 0.5f, 0.5f,    1.0f,0.2f,0.2f,
            -0.5f, 0.5f, 0.5f,    1.0f,0.2f,0.2f,

            -0.5f,-0.5f,-0.5f,    0.2f,1.0f,0.2f,
            -0.5f, 0.5f,-0.5f,    0.2f,1.0f,0.2f,
             0.5f, 0.5f,-0.5f,    0.2f,1.0f,0.2f,
            -0.5f,-0.5f,-0.5f,    0.2f,1.0f,0.2f,
             0.5f, 0.5f,-0.5f,    0.2f,1.0f,0.2f,
             0.5f,-0.5f,-0.5f,    0.2f,1.0f,0.2f,

            -0.5f, 0.5f,-0.5f,    0.2f,0.2f,1.0f,
            -0.5f, 0.5f, 0.5f,    0.2f,0.2f,1.0f,
             0.5f, 0.5f, 0.5f,    0.2f,0.2f,1.0f,
            -0.5f, 0.5f,-0.5f,    0.2f,0.2f,1.0f,
             0.5f, 0.5f, 0.5f,    0.2f,0.2f,1.0f,
             0.5f, 0.5f,-0.5f,    0.2f,0.2f,1.0f,

            -0.5f,-0.5f,-0.5f,    1.0f,1.0f,0.2f,
             0.5f,-0.5f,-0.5f,    1.0f,1.0f,0.2f,
             0.5f,-0.5f, 0.5f,    1.0f,1.0f,0.2f,
            -0.5f,-0.5f,-0.5f,    1.0f,1.0f,0.2f,
             0.5f,-0.5f, 0.5f,    1.0f,1.0f,0.2f,
            -0.5f,-0.5f, 0.5f,    1.0f,1.0f,0.2f,

             0.5f,-0.5f,-0.5f,    0.2f,1.0f,1.0f,
             0.5f, 0.5f,-0.5f,    0.2f,1.0f,1.0f,
             0.5f, 0.5f, 0.5f,    0.2f,1.0f,1.0f,
             0.5f,-0.5f,-0.5f,    0.2f,1.0f,1.0f,
             0.5f, 0.5f, 0.5f,    0.2f,1.0f,1.0f,
             0.5f,-0.5f, 0.5f,    0.2f,1.0f,1.0f,

            -0.5f,-0.5f,-0.5f,    1.0f,0.2f,1.0f,
            -0.5f,-0.5f, 0.5f,    1.0f,0.2f,1.0f,
            -0.5f, 0.5f, 0.5f,    1.0f,0.2f,1.0f,
            -0.5f,-0.5f,-0.5f,    1.0f,0.2f,1.0f,
            -0.5f, 0.5f, 0.5f,    1.0f,0.2f,1.0f,
            -0.5f, 0.5f,-0.5f,    1.0f,0.2f,1.0f,
        };

        m_vao.create();
        QOpenGLVertexArrayObject::Binder binder(&m_vao);

        m_vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        m_vbo->create();
        m_vbo->bind();
        m_vbo->allocate(v, static_cast<int>(sizeof(v)));

        m_program->bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        m_program->release();
        m_vbo->release();
    }

    void resizeGL(int w, int h) override
    {
        m_proj.setToIdentity();
        m_proj.perspective(60.0f, float(w) / float(h ? h : 1), 0.1f, 100.0f);
    }

    void paintGL() override
    {
        glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        QMatrix4x4 view;
        view.translate(0.0f, 0.0f, -2.5f);

        QMatrix4x4 model;
        model.rotate(m_angle, 0.3f, 1.0f, 0.0f);

        const QMatrix4x4 mvp = m_proj * view * model;

        m_program->bind();
        m_program->setUniformValue("uMVP", mvp);

        QOpenGLVertexArrayObject::Binder binder(&m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        m_program->release();
    }

private:
    QMatrix4x4 m_proj;
    float m_angle = 0.0f;

    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLBuffer> m_vbo;
    QOpenGLVertexArrayObject m_vao;
};

int main(int argc, char** argv)
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);

    GLWidget w;
    w.resize(1280, 720);
    w.setWindowTitle("MC Render");
    w.show();

    return app.exec();
}
