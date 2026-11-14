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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    const ProjectContainer &m_project;
    const GameScene &m_scene;
};
