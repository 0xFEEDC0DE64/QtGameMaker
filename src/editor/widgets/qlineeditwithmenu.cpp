#include "qlineeditwithmenu.h"

#include <QMenu>
#include <QMouseEvent>

void QLineEditWithMenu::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);

    if (!m_menu)
        return;

    m_menu->exec(mapToGlobal(event->pos()));
}
