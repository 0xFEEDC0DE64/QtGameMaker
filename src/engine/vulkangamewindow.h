#pragma once

#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

struct ProjectContainer;

class VulkanGameWindow : public QVulkanWindow
{
public:
    explicit VulkanGameWindow(const ProjectContainer &project, QWindow *parent = nullptr);

    QVulkanWindowRenderer *createRenderer() override;

private:
    const ProjectContainer &m_project;
};
