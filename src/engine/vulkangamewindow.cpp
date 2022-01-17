#include "vulkangamewindow.h"

#include "vulkangamerenderer.h"

VulkanGameWindow::VulkanGameWindow(const ProjectContainer &project, const float &rotation, QWindow *parent) :
    QVulkanWindow{parent},
    m_project{project},
    m_rotation{rotation}
{
    setMinimumWidth(640);
    setMaximumWidth(640);
    setMinimumHeight(480);
    setMaximumHeight(480);
    resize(640, 480);
}

QVulkanWindowRenderer *VulkanGameWindow::createRenderer()
{
    return new VulkanGameRenderer{m_project, m_rotation, this, true}; // try MSAA, when available
}
