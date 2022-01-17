#pragma once

#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

struct ProjectContainer;

class VulkanGameWindow : public QVulkanWindow
{
public:
    explicit VulkanGameWindow(const ProjectContainer &project, const float &rotation, QWindow *parent = nullptr);

    QVulkanWindowRenderer *createRenderer() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    const ProjectContainer &m_project;
    const float &m_rotation;
};
