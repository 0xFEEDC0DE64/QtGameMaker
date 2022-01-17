#include "gameengine.h"

#include <QVulkanInstance>
#include <QEventLoop>
#include <QTimer>

#include "closeeventfilter.h"

GameEngine::GameEngine(const ProjectContainer &project, QObject *parent) :
    QObject{parent},
    m_project{project},
    m_glGameWindow{m_project, m_rotation},
    m_vulkanGameWindow{m_project, m_rotation},
    m_timerId(startTimer(1000/60, Qt::PreciseTimer))
{
    m_vulkanInstance.setLayers(QByteArrayList { "VK_LAYER_LUNARG_standard_validation" });

    if (!m_vulkanInstance.create())
        qFatal("Failed to create Vulkan instance: %d", m_vulkanInstance.errorCode());

    m_vulkanGameWindow.setVulkanInstance(&m_vulkanInstance);

    m_glGameWindow.setModality(Qt::WindowModal);
    m_vulkanGameWindow.setModality(Qt::WindowModal);
}

void GameEngine::run()
{
    m_glGameWindow.setTitle(tr("Game Window - OpenGL"));
    m_vulkanGameWindow.setTitle(tr("Game Window - Vulkan"));

    CloseEventFilter closeEventFilter;

    m_glGameWindow.installEventFilter(&closeEventFilter);
    m_vulkanGameWindow.installEventFilter(&closeEventFilter);

    QEventLoop eventLoop;
    connect(&closeEventFilter, &CloseEventFilter::closeEventReceived,
            &eventLoop, &QEventLoop::quit);

    m_glGameWindow.show();
    m_vulkanGameWindow.show();

    QTimer::singleShot(100, &m_glGameWindow, [this](){
        auto geometry = m_glGameWindow.geometry();
        qDebug() << geometry;
        geometry.moveLeft(geometry.left() - (geometry.width() / 2));
        m_glGameWindow.setGeometry(geometry);
    });
    QTimer::singleShot(100, &m_vulkanGameWindow, [this](){
        auto geometry = m_vulkanGameWindow.geometry();
        qDebug() << geometry;
        geometry.moveLeft(geometry.left() + (geometry.width() / 2));
        m_vulkanGameWindow.setGeometry(geometry);
    });

    eventLoop.exec();
}

void GameEngine::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
    {
        m_rotation += 1.f;
    }
    else
        QObject::timerEvent(event);
}
