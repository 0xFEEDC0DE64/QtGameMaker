#include "closeeventfilter.h"

#include <QEvent>

CloseEventFilter::CloseEventFilter(QObject *parent) :
    QObject{parent}
{
}

bool CloseEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Close)
        emit closeEventReceived();

    return QObject::eventFilter(obj, event);
}
