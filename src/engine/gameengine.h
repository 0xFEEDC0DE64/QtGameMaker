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

private:
    const ProjectContainer &m_project;

    QVulkanInstance m_vulkanInstance;

    GlGameWindow m_glGameWindow;
    VulkanGameWindow m_vulkanGameWindow;
};
