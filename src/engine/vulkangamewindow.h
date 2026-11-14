#pragma once

#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

struct ProjectContainer;
struct GameScene;

class VulkanGameWindow : public QVulkanWindow
{
public:
    explicit VulkanGameWindow(const ProjectContainer &project, const GameScene &scene, QWindow *parent = nullptr);

    QVulkanWindowRenderer *createRenderer() override;

private:
    const ProjectContainer &m_project;
    const GameScene &m_scene;
};
