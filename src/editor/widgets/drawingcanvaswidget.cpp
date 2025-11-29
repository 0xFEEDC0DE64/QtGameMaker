#include "drawingcanvaswidget.h"

#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QQueue>
#include <QDebug>

DrawingCanvasWidget::DrawingCanvasWidget(QWidget *parent) :
    QWidget{parent}
{
    setMouseTracking(true);
}

void DrawingCanvasWidget::setMode(Mode mode)
{
    qDebug() << mode;
    if (m_mode != mode)
        emit modeChanged(m_mode = mode);
    // TODO cursor
}

QPixmap DrawingCanvasWidget::pixmap() const
{
    return QPixmap::fromImage(m_image);
}

void DrawingCanvasWidget::setPixmap(const QPixmap &pixmap)
{
    m_image = pixmap.toImage();
    setFixedSize(m_image.size() * m_zoom);
    update();
}

void DrawingCanvasWidget::setScale(float zoom)
{
    if (m_zoom == zoom)
        return;
    emit zoomChanged(m_zoom = zoom);
    setFixedSize(m_image.size() * m_zoom);
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
    QWidget::paintEvent(ev);

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

    painter.drawImage(rect(), m_image);
}

void DrawingCanvasWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    auto zoomdPos = event->pos() / m_zoom;
    auto left = event->button() == Qt::LeftButton;

    switch (event->button())
    {
    case Qt::LeftButton:
    case Qt::RightButton:
    {        
        switch (m_mode)
        {
        case Mode::Draw:
            m_currentlyDrawing = left;
            setPixel(zoomdPos, left);
            break;
        case Mode::Spray:
            break;
        case Mode::Erase:
            break;
        case Mode::Pick:
            break;
        case Mode::Line:
            break;
        case Mode::Polygon:
            break;
        case Mode::Rectangle:
            break;
        case Mode::Ellipse:
            break;
        case Mode::RoundedRectangle:
            break;
        case Mode::SelectRegion:
            break;
        case Mode::SelectWand:
            break;
        case Mode::SelectSpray:
            break;
        case Mode::Text:
            break;
        case Mode::Fill:
            floodFill(zoomdPos, left);
            break;
        case Mode::Replace:
            break;
        };

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

        break;
    }
    default:;
    }
}

void DrawingCanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    auto zoomdPos = event->pos() / m_zoom;
    emit cursorMoved(zoomdPos);

    if (m_currentlyDrawing)
        setPixel(zoomdPos, *m_currentlyDrawing);
}

void DrawingCanvasWidget::setPixel(QPoint pos, bool leftColor)
{
    if (!m_image.rect().contains(pos))
        return;

    m_image.setPixelColor(pos.x(), pos.y(), leftColor ? m_leftButtonColor : m_rightButtonColor);

    repaint();
    emit changed();
}

void DrawingCanvasWidget::floodFill(QPoint pos, bool leftColor)
{
    if (!m_image.rect().contains(pos))
        return;

    const QColor targetColor = m_image.pixelColor(pos);
    const QColor replacementColor = leftColor ? m_leftButtonColor : m_rightButtonColor;

    if (targetColor == replacementColor)
        return;

    QQueue<QPoint> queue;
    queue.enqueue(pos);

    while (!queue.isEmpty())
    {
        QPoint p = queue.dequeue();

        if (m_image.pixelColor(p) != targetColor)
            continue;

        m_image.setPixelColor(p, replacementColor);

        if (p.x() > 0)
            queue.enqueue(QPoint(p.x() - 1, p.y()));
        if (p.x() < m_image.width() - 1)
            queue.enqueue(QPoint(p.x() + 1, p.y()));
        if (p.y() > 0)
            queue.enqueue(QPoint(p.x(), p.y() - 1));
        if (p.y() < m_image.height() - 1)
            queue.enqueue(QPoint(p.x(), p.y() + 1));
    }

    repaint();
    emit changed();
}
