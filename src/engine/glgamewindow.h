#pragma once

#include <vector>

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QWindow>

class QOpenGLContext;
class QOpenGLShaderProgram;
struct GameScene;
struct ProjectContainer;

class GlGameWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GlGameWindow(const ProjectContainer &project, const GameScene &scene, QWindow *parent = nullptr);
    ~GlGameWindow() override;

    void renderNow();

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    bool initialize();
    void releaseResources();

    const ProjectContainer &m_project;
    const GameScene &m_scene;
    QOpenGLContext *m_context{};
    QOpenGLShaderProgram *m_program{};
    QOpenGLVertexArrayObject m_vao;
    GLuint m_vertexBuffer{};
    std::vector<GLuint> m_textures;
};
