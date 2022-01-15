#include "gameengine.h"

#include <QVulkanInstance>
#include <QEventLoop>

#include "closeeventfilter.h"

GameEngine::GameEngine(const ProjectContainer &project, QObject *parent) :
    QObject{parent},
    m_project{project},
    m_glGameWindow{m_project},
    m_vulkanGameWindow{m_project}
{
    m_vulkanInstance.setLayers(QByteArrayList { "VK_LAYER_LUNARG_standard_validation" });

    if (!m_vulkanInstance.create())
        qFatal("Failed to create Vulkan instance: %d", m_vulkanInstance.errorCode());

    m_vulkanGameWindow.setVulkanInstance(&m_vulkanInstance);
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

    eventLoop.exec();
}
