#include "zoomablescrollarea.h"

#include <QWheelEvent>

#include "drawingcanvaswidget.h"

void ZoomableScrollArea::wheelEvent(QWheelEvent *event)
{
    // Only zoom when Ctrl is held
    if (!event->modifiers().testFlag(Qt::ControlModifier))
    {
        QScrollArea::wheelEvent(event);
        return;
    }

    auto *w = qobject_cast<DrawingCanvasWidget*>(widget());
    if (!w)
        return;

    auto zoom = w->zoom();
    if (event->angleDelta().y() > 0)
        zoom *= 2;
    else
        zoom /= 2;

    w->setZoom(zoom);

    event->accept();
}
