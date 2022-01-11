#include "drawingcanvaswidget.h"

#include <QPixmap>
#include <QPainter>

DrawingCanvasWidget::DrawingCanvasWidget(QWidget *parent) :
    QWidget{parent}
{
}

void DrawingCanvasWidget::setPixmap(QPixmap &pixmap)
{
    m_pixmap = &pixmap;
    setFixedSize(m_pixmap->size() * m_scale);
    update();
}

void DrawingCanvasWidget::setScale(float scale)
{
    if (m_scale == scale)
        return;
    emit scaleChanged(m_scale = scale);
    if (m_pixmap)
        setFixedSize(m_pixmap->size() * m_scale);
    update();
}

void DrawingCanvasWidget::paintEvent(QPaintEvent *ev)
{
    if (!m_pixmap)
    {
        QWidget::paintEvent(ev);
        return;
    }

    QBrush brush;
    {
        QPixmap pixmap{32, 32};
        {
            QPainter painter{&pixmap};
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::gray);
            painter.drawRect(0, 0, 16, 16);
            painter.drawRect(16, 16, 16, 16);
            painter.setBrush(Qt::white);
            painter.drawRect(0, 16, 16, 16);
            painter.drawRect(16, 0, 16, 16);
        }
        brush.setTexture(std::move(pixmap));
    }

    QPainter painter{this};

    painter.setPen(Qt::NoPen);
    painter.setBrush(std::move(brush));
    painter.drawRect(rect());

    painter.drawPixmap(rect(), *m_pixmap);
}
