#include "vulkangamewindow.h"

#include <QKeyEvent>

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

void VulkanGameWindow::keyPressEvent(QKeyEvent *event)
{
    QVulkanWindow::keyPressEvent(event);

    switch (event->key())
    {
    case Qt::Key_Escape:
        close();
        break;
    }
}

void VulkanGameWindow::keyReleaseEvent(QKeyEvent *event)
{
    QVulkanWindow::keyReleaseEvent(event);
}
