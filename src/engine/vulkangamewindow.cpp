#include "vulkangamewindow.h"

#include "vulkangamerenderer.h"

VulkanGameWindow::VulkanGameWindow(const ProjectContainer &project, QWindow *parent) :
    QVulkanWindow{parent},
    m_project{project}
{
    setMinimumWidth(640);
    setMaximumWidth(640);
    setMinimumHeight(480);
    setMaximumHeight(480);
}

QVulkanWindowRenderer *VulkanGameWindow::createRenderer()
{
    return new VulkanGameRenderer(this, true); // try MSAA, when available
}
