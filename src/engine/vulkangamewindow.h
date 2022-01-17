#pragma once

#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

struct ProjectContainer;

class VulkanGameWindow : public QVulkanWindow
{
public:
    explicit VulkanGameWindow(const ProjectContainer &project, const float &rotation, QWindow *parent = nullptr);

    QVulkanWindowRenderer *createRenderer() override;

private:
    const ProjectContainer &m_project;
    const float &m_rotation;
};
