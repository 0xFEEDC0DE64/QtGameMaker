#pragma once

#include <array>

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QVulkanInstance>

#include "engine/gamescene.h"
#include "engine/glgamewindow.h"
#include "engine/vulkangamewindow.h"

struct ProjectContainer;

class GameEngine : public QObject
{
    Q_OBJECT

public:
    explicit GameEngine(const ProjectContainer &project, QObject *parent = nullptr);
    void run();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    const ProjectContainer &m_project;
    GameScene m_scene;
    QVulkanInstance m_vulkanInstance;
    GlGameWindow m_glGameWindow;
    VulkanGameWindow m_vulkanGameWindow;
    bool m_vulkanAvailable{};
    QWindow *m_mouseSource{};
    QTimer m_frameTimer;
    QElapsedTimer m_elapsed;
    qint64 m_nextFrameNanoseconds{};
    std::array<bool, std::variant_size_v<Action>> m_warnedActionTypes{};
};
