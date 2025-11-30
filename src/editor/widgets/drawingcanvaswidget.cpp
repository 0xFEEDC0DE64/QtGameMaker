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

void DrawingCanvasWidget::setZoom(float zoom)
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

void DrawingCanvasWidget::setTransparentBackgroundPattern(const TransparentBackgroundPattern &transparentBackgroundPattern)
{
    if (m_transparentBackgroundPattern == transparentBackgroundPattern)
        return;

    emit transparentBackgroundPatternChanged(m_transparentBackgroundPattern = transparentBackgroundPattern);
    update();
}

void DrawingCanvasWidget::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);

    const auto brush = makeTransparentBackgroundPatternBrush(m_transparentBackgroundPattern);

    QPainter painter{this};

    painter.setPen(Qt::NoPen);
    painter.setBrush(std::move(brush));
    painter.drawRect(rect());

    painter.drawImage(rect(), m_image);
}

void DrawingCanvasWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QPoint zoomdPos(
        std::round(event->pos().x() / m_zoom),
        std::round(event->pos().y() / m_zoom)
    );
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
            //TODO
            break;
        case Mode::Erase:
            //TODO
            break;
        case Mode::Pick:
            //TODO
            break;
        case Mode::Line:
            //TODO
            break;
        case Mode::Polygon:
            //TODO
            break;
        case Mode::Rectangle:
            //TODO
            break;
        case Mode::Ellipse:
            //TODO
            break;
        case Mode::RoundedRectangle:
            //TODO
            break;
        case Mode::SelectRegion:
            //TODO
            break;
        case Mode::SelectWand:
            //TODO
            break;
        case Mode::SelectSpray:
            //TODO
            break;
        case Mode::Text:
            //TODO
            break;
        case Mode::Fill:
            floodFill(zoomdPos, left);
            break;
        case Mode::Replace:
            //TODO
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

    QPoint zoomdPos(
        std::round(event->pos().x() / m_zoom),
        std::round(event->pos().y() / m_zoom)
    );
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
