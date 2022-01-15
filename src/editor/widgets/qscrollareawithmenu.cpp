#include "qscrollareawithmenu.h"

#include <QMenu>
#include <QMouseEvent>

void QScrollAreaWithMenu::mousePressEvent(QMouseEvent *event)
{
    QScrollArea::mousePressEvent(event);

    if (!m_menu)
        return;

    m_menu->exec(mapToGlobal(event->pos()));
}
