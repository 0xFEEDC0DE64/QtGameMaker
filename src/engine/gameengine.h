#pragma once

#include <QObject>

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
    void timerEvent(QTimerEvent *event) override;

private:
    const ProjectContainer &m_project;

    QVulkanInstance m_vulkanInstance;

    float m_rotation{};

    GlGameWindow m_glGameWindow;
    VulkanGameWindow m_vulkanGameWindow;

    const int m_timerId;
};
