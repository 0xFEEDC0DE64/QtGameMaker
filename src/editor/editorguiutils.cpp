#include "editorguiutils.h"

#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QColor>

QBrush makeGridBrush(int snapX, int snapY, const QPen &gridPen, const QColor &backgroundColor)
{
    QPixmap pixmap{snapX, snapY};
    pixmap.fill(backgroundColor);

    {
        QPainter painter{&pixmap};
        painter.setPen(gridPen);
        painter.drawLine(0, 0, snapX, 0);
        painter.drawLine(0, 0, 0, snapY);
    }

    return QBrush{std::move(pixmap)};
}

QBrush makeGridBrush(int snapX, int snapY, const QColor &gridColor, const QColor &backgroundColor)
{
    QPixmap pixmap{snapX, snapY};
    pixmap.fill(backgroundColor);

    {
        QPainter painter{&pixmap};
        painter.setPen(gridColor);
        painter.drawLine(0, 0, snapX, 0);
        painter.drawLine(0, 0, 0, snapY);
    }

    return QBrush{std::move(pixmap)};
}
