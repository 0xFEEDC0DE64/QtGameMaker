#include "vulkangamewindow.h"

#include "projectcontainer.h"
#include "gamescene.h"
#include "vulkangamerenderer.h"

VulkanGameWindow::VulkanGameWindow(const ProjectContainer &project, const GameScene &scene, QWindow *parent) :
    QVulkanWindow{parent},
    m_project{project},
    m_scene{scene}
{
    setPreferredColorFormats({VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM});
    if (!m_project.rooms.empty())
    {
        resize(m_scene.roomSize);
        if (!m_project.globalGameSettings.resizable)
        {
            setMinimumSize(m_scene.roomSize);
            setMaximumSize(m_scene.roomSize);
        }
        setTitle(m_scene.caption);
    }
}

QVulkanWindowRenderer *VulkanGameWindow::createRenderer()
{
    return new VulkanGameRenderer{m_scene, this};
}
