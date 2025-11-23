#include "drawingcanvaswidget.h"

#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

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

void DrawingCanvasWidget::setLeftButtonColor(const QColor &leftButtonColor)
{
    if (m_leftButtonColor == leftButtonColor)
        return;
    emit leftButtonColorChanged(m_leftButtonColor = leftButtonColor);
}

void DrawingCanvasWidget::setRightButtonColor(const QColor &rightButtonColor)
{
    if (m_rightButtonColor == rightButtonColor)
        return;
    emit rightButtonColorChanged(m_rightButtonColor = rightButtonColor);
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

void DrawingCanvasWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
    case Qt::RightButton:
    {
        m_currentlyDrawing = event->button() == Qt::LeftButton;

        setMouseTracking(true);

        setPixel(event->pos() / m_scale, event->button() == Qt::LeftButton);

        break;
    }
    default:;
    }
}

void DrawingCanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
    case Qt::RightButton:
    {
        m_currentlyDrawing = std::nullopt;

        setMouseTracking(false);

        break;
    }
    default:;
    }
}

void DrawingCanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (m_currentlyDrawing)
        setPixel(event->pos() / m_scale, *m_currentlyDrawing);
}

void DrawingCanvasWidget::setPixel(QPoint pos, bool leftColor)
{
    auto image = m_pixmap->toImage();
    image.setPixelColor(pos.x(), pos.y(), leftColor ? m_leftButtonColor : m_rightButtonColor);
    *m_pixmap = QPixmap::fromImage(image);
    repaint();
    emit changed();
}
