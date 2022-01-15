#pragma once

#include <QWindow>
#include <QOpenGLFunctions>

class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLShaderProgram;
struct ProjectContainer;

class GlGameWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GlGameWindow(const ProjectContainer &project, QWindow *parent = nullptr);
    ~GlGameWindow();

    void initialize();
    void render();

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void exposeEvent(QExposeEvent *event) override;

private:
    const ProjectContainer &m_project;

    QOpenGLContext *m_context{};
    QOpenGLPaintDevice *m_device{};

    GLint m_posAttr = 0;
    GLint m_colAttr = 0;
    GLint m_matrixUniform = 0;

    QOpenGLShaderProgram *m_program{};
    int m_frame{};
};
