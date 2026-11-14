#include "gameengine.h"

#include <algorithm>

#include <QEventLoop>
#include <QEnterEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

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
    m_glGameWindow.installEventFilter(this);
    m_vulkanGameWindow.installEventFilter(this);

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

bool GameEngine::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != &m_glGameWindow && watched != &m_vulkanGameWindow)
        return QObject::eventFilter(watched, event);

    auto *window = static_cast<QWindow *>(watched);
    const auto roomPosition = [this, window](QPointF position) {
        return QPointF{position.x() * m_scene.roomSize.width() / std::max(1, window->width()),
                       position.y() * m_scene.roomSize.height() / std::max(1, window->height())};
    };

    switch (event->type())
    {
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape &&
            m_project.globalGameSettings.letEscEndGame)
        {
            window->close();
            return true;
        }
        break;
    case QEvent::FocusIn:
        if (m_mouseSource != window)
        {
            m_scene.resetMouseInput();
            m_mouseSource = window;
        }
        break;
    case QEvent::FocusOut:
    case QEvent::Hide:
        if (m_mouseSource == window)
        {
            m_scene.resetMouseInput();
            m_mouseSource = nullptr;
        }
        break;
    case QEvent::Enter:
        if (!m_mouseSource)
            m_mouseSource = window;
        if (m_mouseSource == window)
            m_scene.queueMouseMove(roomPosition(static_cast<QEnterEvent *>(event)->position()));
        break;
    case QEvent::Leave:
        if (m_mouseSource == window)
            m_scene.queueMouseLeave();
        break;
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)
        {
            if (m_mouseSource != window)
            {
                m_scene.resetMouseInput();
                m_mouseSource = window;
            }
        }
        if (m_mouseSource != window)
            break;
        const QPointF position = roomPosition(mouse->position());
        if (event->type() == QEvent::MouseMove)
            m_scene.queueMouseMove(position);
        else
            m_scene.queueMouseButton(position, mouse->button(),
                                     event->type() != QEvent::MouseButtonRelease);
        break;
    }
    case QEvent::Wheel:
        if (m_mouseSource == window)
        {
            auto *wheel = static_cast<QWheelEvent *>(event);
            m_scene.queueMouseWheel(roomPosition(wheel->position()), wheel->angleDelta().y());
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
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
