#include "roomeditwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

RoomEditWidget::RoomEditWidget(QWidget *parent) :
    QWidget{parent}
{
    setBackgroundRole(QPalette::Light);
    setMouseTracking(true);
    setFixedSize(640, 480);
}

void RoomEditWidget::setSnapX(int snapX)
{
    if (m_snapX == snapX)
        return;
    emit snapXChanged(m_snapX = snapX);
    update();
}

void RoomEditWidget::setSnapY(int snapY)
{
    if (m_snapY == snapY)
        return;
    emit snapYChanged(m_snapY = snapY);
    update();
}

void RoomEditWidget::setGridEnabled(bool gridEnabled)
{
    if (m_gridEnabled == gridEnabled)
        return;
    emit gridEnabledChanged(m_gridEnabled = gridEnabled);
    update();
}

void RoomEditWidget::setIsometricGrid(bool isometricGrid)
{
    if (m_isometricGrid == isometricGrid)
        return;
    emit isometricGridChanged(m_isometricGrid = isometricGrid);
    update();
}

void RoomEditWidget::setGridRole(QPalette::ColorRole gridRole)
{
    if (gridRole == m_gridRole)
        return;
    m_gridRole = gridRole;
    update();
}

void RoomEditWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (m_gridEnabled)
    {
        if (!m_gridBrush || m_gridBrush->snapX != m_snapX || m_gridBrush->snapY != m_snapY)
        {
            QPixmap pixmap{m_snapX, m_snapY};

            {
                QPainter painter{&pixmap};
                painter.setPen(palette().color(m_gridRole));
                painter.drawLine(0, 0, m_snapX, 0);
                painter.drawLine(0, 0, 0, m_snapY);

                painter.fillRect(1, 1, m_snapX - 1, m_snapY - 1, palette().color(backgroundRole()));
            }

            m_gridBrush = GridBrush {
                .snapX = m_snapX,
                .snapY = m_snapY,
                .brush = QBrush{std::   move(pixmap)}
            };
        }
    }
    else
        m_gridBrush = std::nullopt;

    QPainter painter{this};
    painter.fillRect(rect(), m_gridBrush ? m_gridBrush->brush : palette().color(backgroundRole()));
}

void RoomEditWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void RoomEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void RoomEditWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    emit cursorMoved(snapPoint(event->pos()));
}

QPoint RoomEditWidget::snapPoint(const QPoint &point) const
{
    return QPoint{
        m_snapX > 1 ? ((point.x() + (m_snapX / 2)) / m_snapX * m_snapX) : point.x(),
        m_snapY > 1 ? ((point.y() + (m_snapY / 2)) / m_snapY * m_snapY) : point.y(),
    };
}
