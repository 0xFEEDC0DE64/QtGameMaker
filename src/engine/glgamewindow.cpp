#include "glgamewindow.h"

#include <array>
#include <cstddef>

#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QPlatformSurfaceEvent>

#include "gamescene.h"
#include "projectcontainer.h"

namespace {

struct Vertex
{
    float x, y, z, u, v;
};

constexpr std::array<Vertex, 4> quad {{
    {0.f, 0.f, 0.f, 0.f, 0.f},
    {0.f, 1.f, 0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f, 1.f, 0.f},
    {1.f, 1.f, 0.f, 1.f, 1.f}
}};

}

GlGameWindow::GlGameWindow(const ProjectContainer &project, const GameScene &scene, QWindow *parent) :
    QWindow{parent},
    m_project{project},
    m_scene{scene}
{
    setSurfaceType(QWindow::OpenGLSurface);
    if (!m_project.rooms.empty())
    {
        resize(m_scene.roomSize);
        if (!m_project.globalGameSettings.resizable)
        {
            setMinimumSize(m_scene.roomSize);
            setMaximumSize(m_scene.roomSize);
        }
        setTitle(m_scene.caption);
    }
}

GlGameWindow::~GlGameWindow()
{
    releaseResources();
}

bool GlGameWindow::initialize()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram;
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             QStringLiteral(":/qtgameengine/opengl_shaders/texture.vert")) ||
        !m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             QStringLiteral(":/qtgameengine/opengl_shaders/texture.frag")) ||
        !m_program->link())
    {
        qWarning("Could not compile OpenGL sprite shaders: %s", qPrintable(m_program->log()));
        return false;
    }

    if (!m_vao.create())
    {
        qWarning("Could not create OpenGL vertex array");
        return false;
    }
    m_vao.bind();
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad.data(), GL_STATIC_DRAW);

    m_program->bind();
    const int position = m_program->attributeLocation("position");
    const int texCoord = m_program->attributeLocation("texCoord");
    if (position < 0 || texCoord < 0)
    {
        qWarning("OpenGL sprite shader has missing attributes");
        return false;
    }
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(texCoord);
    glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(offsetof(Vertex, u)));
    m_program->setUniformValue("tex", 0);
    m_program->release();
    m_vao.release();

    m_textures.reserve(m_scene.textures.size());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (const QImage &source : m_scene.textures)
    {
        const QImage image = source.convertToFormat(QImage::Format_RGBA8888);
        GLuint texture{};
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());
        m_textures.push_back(texture);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void GlGameWindow::releaseResources()
{
    if (!m_context)
        return;

    if (m_context->makeCurrent(this))
    {
        if (!m_textures.empty())
            glDeleteTextures(static_cast<GLsizei>(m_textures.size()), m_textures.data());
        if (m_vertexBuffer)
            glDeleteBuffers(1, &m_vertexBuffer);
        m_vao.destroy();
        delete m_program;
        m_program = nullptr;
        m_context->doneCurrent();
    }
    else
        delete m_program;

    m_textures.clear();
    m_vertexBuffer = 0;
    m_program = nullptr;
    delete m_context;
    m_context = nullptr;
}

void GlGameWindow::renderNow()
{
    if (!isExposed() || m_project.rooms.empty())
        return;

    if (!m_context)
    {
        m_context = new QOpenGLContext;
        m_context->setFormat(requestedFormat());
        if (!m_context->create())
        {
            qWarning("Could not create OpenGL context for the game window");
            delete m_context;
            m_context = nullptr;
            return;
        }
        if (!m_context->makeCurrent(this) || !initialize())
        {
            releaseResources();
            return;
        }
    }
    else if (!m_context->makeCurrent(this))
        return;

    const qreal scale = devicePixelRatio();
    glViewport(0, 0, qRound(width() * scale), qRound(height() * scale));
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QMatrix4x4 projection;
    projection.ortho(0.f, float(m_scene.roomSize.width()),
                     float(m_scene.roomSize.height()), 0.f, -1.f, 1.f);
    m_program->bind();
    m_vao.bind();
    glActiveTexture(GL_TEXTURE0);
    for (const GameScene::Instance *instance : m_scene.drawOrder())
    {
        if (!instance->visible || !instance->spriteIndex)
            continue;
        const std::size_t textureIndex = m_scene.textureFor(*instance);
        const QImage &image = m_scene.textures.at(textureIndex);
        const QPoint &origin = m_scene.sprites.at(*instance->spriteIndex).origin;

        QMatrix4x4 matrix = projection;
        matrix.translate(float(instance->position.x() - origin.x()),
                         float(instance->position.y() - origin.y()));
        matrix.scale(float(image.width()), float(image.height()));
        m_program->setUniformValue("matrix", matrix);
        glBindTexture(GL_TEXTURE_2D, m_textures.at(textureIndex));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    m_vao.release();
    m_program->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_context->swapBuffers(this);
}

bool GlGameWindow::event(QEvent *event)
{
    if (event->type() == QEvent::PlatformSurface &&
        static_cast<QPlatformSurfaceEvent *>(event)->surfaceEventType() ==
            QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
        releaseResources();

    if (event->type() == QEvent::UpdateRequest)
    {
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

void GlGameWindow::exposeEvent(QExposeEvent *event)
{
    QWindow::exposeEvent(event);
    if (isExposed())
        renderNow();
}
