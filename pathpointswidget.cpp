#include "pathpointswidget.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

#include <algorithm>

PathPointsWidget::PathPointsWidget(QWidget *parent) :
    QWidget{parent}
{
    setMouseTracking(true);
}

PathPointsWidget::PathPointsWidget(std::vector<Path::Point> *points, QWidget *parent) :
    QWidget{parent},
    m_points{points}
{
}

void PathPointsWidget::setPoints(std::vector<Path::Point> *points)
{
    m_points = points;
    update();
}

void PathPointsWidget::setShowGrid(bool showGrid)
{
    if (m_showGrid == showGrid)
        return;
    emit showGridChanged(m_showGrid = showGrid);
    update();
}

void PathPointsWidget::setGridX(int gridX)
{
    if (m_gridX == gridX)
        return;
    emit gridXChanged(m_gridX = gridX);
    update();
}

void PathPointsWidget::setGridY(int gridY)
{
    if (m_gridY == gridY)
        return;
    emit gridYChanged(m_gridY = gridY);
    update();
}

void PathPointsWidget::setClosed(bool closed)
{
    if (m_closed == closed)
        return;
    emit closedChanged(m_closed = closed);
    update();
}

void PathPointsWidget::setSelectedIndex(const std::optional<std::size_t> &selectedIndex)
{
    if (m_selectedIndex == selectedIndex)
        return;
    emit selectedIndexChanged(m_selectedIndex = selectedIndex);
    update();
}

void PathPointsWidget::paintEvent(QPaintEvent *event)
{
    if (m_showGrid)
    {
        if (!m_gridBrush || m_gridBrush->gridX != m_gridX || m_gridBrush->gridY != m_gridY)
        {
            QPixmap pixmap{m_gridX, m_gridY};

            {
                QPainter painter{&pixmap};
                painter.setPen(palette().color(m_gridRole));
                painter.drawLine(0, 0, m_gridX, 0);
                painter.drawLine(0, 0, 0, m_gridY);

                painter.fillRect(1, 1, m_gridX - 1, m_gridY - 1, palette().color(backgroundRole()));
            }

            qDebug() << "setup brush" << pixmap.width() << pixmap.height();

            m_gridBrush = GridBrush {
                .gridX = m_gridX,
                .gridY = m_gridY,
                .brush = QBrush{std::   move(pixmap)}
            };
        }
    }
    else
        m_gridBrush = std::nullopt;

    QPainter painter{this};
    painter.fillRect(rect(), m_gridBrush ? m_gridBrush->brush : palette().color(backgroundRole()));

    if (!m_points)
        return;

    const auto drawLine = [&](const auto &point0, const auto &point1){
        painter.setPen(QPen{Qt::black, 2});
        painter.drawLine(point0.point, point1.point);
        painter.setPen(QPen{Qt::yellow, 1});
        painter.drawLine(point0.point, point1.point);
    };

    std::adjacent_find(std::cbegin(*m_points), std::cend(*m_points),
                       [&](const Path::Point &point0, const Path::Point &point1){
        drawLine(point0, point1);
        return false;
    });

    if (m_closed && m_points->size() >= 2)
        drawLine(m_points->back(), m_points->front());

    painter.setPen(Qt::black);
    std::size_t i{};
    for (const auto &point : *m_points)
    {
        if (i == m_selectedIndex)
            painter.setBrush(Qt::red);
        else if (i == 0)
            painter.setBrush(Qt::green);
        else
            painter.setBrush(Qt::blue);
        painter.drawEllipse(point.point, 2, 2);
        i++;
    }
}

void PathPointsWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
        if (!m_points)
            return;

        if (!m_points->empty())
        {
            auto points = *m_points;
            const auto distance = [&](const Path::Point &point){ return QLineF{point.point, event->pos()}.length(); };
            std::sort(std::begin(points), std::end(points), [&](const Path::Point &left, const Path::Point &right){
                return distance(left) < distance(right);
            });
            if (distance(points.front()) > 8)
                goto createNew;

            const auto iter = std::find_if(std::cbegin(*m_points), std::cend(*m_points),
                                           [&minPoint=points.front().point](const Path::Point &point){ return point.point == minPoint; });
            if (iter == std::cend(*m_points))
            {
                qWarning() << "unexpected end";
                goto createNew;
            }

            const auto index = std::distance(std::cbegin(*m_points), iter);
            emit pointInserted(index);
            emit selectedIndexChanged(m_selectedIndex = index);
            m_dragIndex = index;
        }
        else
        {
createNew:
            m_points->push_back(Path::Point{.point = snapPoint(event->pos()), .sp=100});
            const auto index = m_points->size() - 1;
            emit pointInserted(index);
            emit selectedIndexChanged(m_selectedIndex = index);
            m_dragIndex = index;
        }
        update();
        break;
    }
}

void PathPointsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    switch (event->button())
    {
    case Qt::LeftButton:
        m_dragIndex = std::nullopt;
        break;
    }
}

void PathPointsWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    emit cursorMoved(snapPoint(event->pos()));

    if (m_dragIndex)
    {
        if (event->buttons().testFlag(Qt::LeftButton))
        {
            const auto point = snapPoint(event->pos());
            if ((*m_points)[*m_dragIndex].point != point)
            {
                (*m_points)[*m_dragIndex].point = point;
                emit pointMoved(*m_dragIndex);
                update();
            }
        }
        else
            m_dragIndex = std::nullopt;
    }
}

QPoint PathPointsWidget::snapPoint(const QPoint &point) const
{
    if (!m_showGrid)
        return point;
    return QPoint{
        (point.x() + (m_gridX / 2)) / m_gridX * m_gridX,
        (point.y() + (m_gridY / 2)) / m_gridY * m_gridY,
    };
}
