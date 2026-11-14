#include "gameengine.h"

#include <algorithm>

#include <QEventLoop>

#include "closeeventfilter.h"
#include "projectcontainer.h"

GameEngine::GameEngine(const ProjectContainer &project, QObject *parent) :
    QObject{parent},
    m_project{project},
    m_scene{project},
    m_glGameWindow{m_project, m_scene},
    m_vulkanGameWindow{m_project, m_scene}
{
    m_vulkanAvailable = m_vulkanInstance.create();
    if (m_vulkanAvailable)
        m_vulkanGameWindow.setVulkanInstance(&m_vulkanInstance);
    else
        qWarning("Could not create Vulkan instance: %d", m_vulkanInstance.errorCode());

    m_glGameWindow.setModality(Qt::WindowModal);
    m_vulkanGameWindow.setModality(Qt::WindowModal);

    m_frameTimer.setTimerType(Qt::PreciseTimer);
    m_frameTimer.setSingleShot(true);
    connect(&m_frameTimer, &QTimer::timeout, this, [this]() {
        constexpr qint64 frameNanoseconds = 1000000000 / 60;
        m_scene.advanceTo(m_elapsed.elapsed());
        m_scene.draw();
        m_glGameWindow.requestUpdate();
        if (m_vulkanAvailable)
            m_vulkanGameWindow.requestUpdate();

        const qint64 now = m_elapsed.nsecsElapsed();
        m_nextFrameNanoseconds = std::max(m_nextFrameNanoseconds + frameNanoseconds,
                                          (now / frameNanoseconds + 1) * frameNanoseconds);
        const qint64 remaining = m_nextFrameNanoseconds - m_elapsed.nsecsElapsed();
        m_frameTimer.start(int(std::max<qint64>(1, (remaining + 999999) / 1000000)));
    });
}

void GameEngine::run()
{
    if (m_project.rooms.empty())
        return;

    m_scene.start([this](ObjectInstance &target, const Action &action, ObjectInstance *) {
        if (target.applyAction(action))
            return;
        static constexpr std::array actionNames{
            "MoveFixed", "MoveFree", "MoveTowards", "SpeedHorizontal",
            "SpeedVertical", "SetGravity", "ExecuteCode"
        };
        const std::size_t type = action.index();
        if (!m_warnedActionTypes[type])
        {
            m_warnedActionTypes[type] = true;
            qWarning("Game action %s has no runtime implementation yet", actionNames[type]);
        }
    });

    CloseEventFilter closeEventFilter;
    m_glGameWindow.installEventFilter(&closeEventFilter);
    if (m_vulkanAvailable)
        m_vulkanGameWindow.installEventFilter(&closeEventFilter);

    QEventLoop eventLoop;
    bool shuttingDown = false;
    connect(&closeEventFilter, &CloseEventFilter::closeEventReceived,
            &eventLoop, [this, &eventLoop, &shuttingDown]() {
        if (shuttingDown)
            return;
        shuttingDown = true;
        m_frameTimer.stop();

        // Finish both QWindow close paths before their Vulkan instance is destroyed.
        if (m_vulkanAvailable)
            m_vulkanGameWindow.close();
        m_glGameWindow.close();
        QTimer::singleShot(0, &eventLoop, &QEventLoop::quit);
    }, Qt::QueuedConnection);

    m_glGameWindow.setTitle(m_glGameWindow.title() + tr(" - OpenGL"));
    m_glGameWindow.show();
    if (m_vulkanAvailable)
    {
        m_vulkanGameWindow.setTitle(m_vulkanGameWindow.title() + tr(" - Vulkan"));
        m_vulkanGameWindow.show();
    }
    m_elapsed.start();
    m_nextFrameNanoseconds = 1000000000 / 60;
    m_frameTimer.start(17);
    eventLoop.exec();
}
