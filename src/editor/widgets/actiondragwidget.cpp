#include "actiondragwidget.h"

#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>

#include "projectserialization.h"
#include "stdserialization.h"

void ActionDragWidget::mousePressEvent(QMouseEvent *event)
{
    QToolButton::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
        m_dragStartPosition = event->pos();
}

void ActionDragWidget::mouseMoveEvent(QMouseEvent *event)
{
    QToolButton::mouseMoveEvent(event);

    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - m_dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag{this};
    drag->setPixmap(this->icon().pixmap(QSize{32, 32}));

    QMimeData *mimeData = new QMimeData;
    QByteArray encoded;
    QDataStream stream(&encoded, QDataStream::WriteOnly);
    stream << m_action;
    mimeData->setData("custom", encoded);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
    qDebug() << dropAction;
}
