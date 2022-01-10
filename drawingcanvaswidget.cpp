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
    QWidget::paintEvent(ev);

    if (!m_pixmap)
        return;

    QPainter painter{this};
    painter.drawPixmap(QRect{QPoint{}, m_pixmap->size() * m_scale}, *m_pixmap);
}
