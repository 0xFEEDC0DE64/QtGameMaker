#include "glgamewindow.h"

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QScreen>
#include <QKeyEvent>
#include <QDebug>

GlGameWindow::GlGameWindow(const ProjectContainer &project, const float &rotation, QWindow *parent) :
    QWindow{parent},
    m_project{project},
    m_rotation{rotation}
{
    setSurfaceType(QWindow::OpenGLSurface);

    setMinimumWidth(640);
    setMaximumWidth(640);
    setMinimumHeight(480);
    setMaximumHeight(480);
    resize(640, 480);
}

GlGameWindow::~GlGameWindow() = default;

void GlGameWindow::initialize()
{
    m_program = new QOpenGLShaderProgram{this};


    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/qtgameengine/opengl_shaders/color.vert")))
        qFatal("could not load vertex shader");
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/qtgameengine/opengl_shaders/color.frag")))
        qFatal("could not load fragment shader");


    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);
}

void GlGameWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(m_rotation, 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    static const GLfloat vertices[] {
        0.0f,  0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    static const GLfloat colors[] {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colAttr);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);

    m_program->release();
}

void GlGameWindow::renderLater()
{
    requestUpdate();
}

void GlGameWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    if (!m_context)
    {
        m_context = new QOpenGLContext{this};
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize)
    {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    renderLater();
}

bool GlGameWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GlGameWindow::keyPressEvent(QKeyEvent *event)
{
    QWindow::keyPressEvent(event);

    switch (event->key())
    {
    case Qt::Key_Escape:
        close();
        break;
    }
}

void GlGameWindow::keyReleaseEvent(QKeyEvent *event)
{
    QWindow::keyReleaseEvent(event);
}

void GlGameWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}
