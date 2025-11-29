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

    auto scale = w->scale();
    if (event->angleDelta().y() > 0)
        scale *= 2;
    else
        scale /= 2;

    w->setScale(scale);

    event->accept();
}
