#include "colorwidget.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>

ColorWidget::ColorWidget(QWidget *parent) :
    QFrame{parent}
{
    setAttribute(Qt::WA_NoSystemBackground);
}

ColorWidget::~ColorWidget() = default;

void ColorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    drawFrame(&p);
    QRect r = contentsRect();
    p.drawPixmap(r.topLeft(), m_pixmap);
}

void ColorWidget::mousePressEvent(QMouseEvent *event)
{
    QFrame::mousePressEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
    case Qt::RightButton:
    {
        m_currentlyPicking = event->button() == Qt::LeftButton;

        setMouseTracking(true);

        pickPixel(event->pos(), event->button() == Qt::LeftButton);

        break;
    }
    default:;
    }
}

void ColorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
    case Qt::RightButton:
    {
        m_currentlyPicking = std::nullopt;

        setMouseTracking(false);

        break;
    }
    default:;
    }
}

void ColorWidget::mouseMoveEvent(QMouseEvent *event)
{
    QFrame::mouseMoveEvent(event);

    if (m_currentlyPicking)
        pickPixel(event->pos(), *m_currentlyPicking);
}

void ColorWidget::pickPixel(const QPoint &pos, bool left)
{
    if (!m_image.rect().contains(pos))
        return;

    if (left)
        emit colorLeftClicked(m_image.pixelColor(pos));
    else
        emit colorRightClicked(m_image.pixelColor(pos));
}
