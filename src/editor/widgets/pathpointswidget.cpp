#include "pathpointswidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include <algorithm>
#include <ranges>

#include "editorguiutils.h"

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

void PathPointsWidget::setSnapX(int snapX)
{
    if (m_snapX == snapX)
        return;
    emit snapXChanged(m_snapX = snapX);
    update();
}

void PathPointsWidget::setSnapY(int snapY)
{
    if (m_snapY == snapY)
        return;
    emit snapYChanged(m_snapY = snapY);
    update();
}

void PathPointsWidget::setGridEnabled(bool gridEnabled)
{
    if (m_gridEnabled == gridEnabled)
        return;
    emit gridEnabledChanged(m_gridEnabled = gridEnabled);
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

void PathPointsWidget::setGridRole(QPalette::ColorRole gridRole)
{
    if (gridRole == m_gridRole)
        return;
    m_gridRole = gridRole;
    update();
}

void PathPointsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (m_gridEnabled)
    {
        if (!m_gridBrush || m_gridBrush->snapX != m_snapX || m_gridBrush->snapY != m_snapY)
        {
            m_gridBrush = GridBrush {
                .snapX = m_snapX,
                .snapY = m_snapY,
                .brush = makeGridBrush(m_snapX, m_snapY, palette().color(m_gridRole), palette().color(backgroundRole()))
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

    for (auto&& [p0, p1] : std::views::adjacent<2>(*m_points))
        drawLine(p0, p1);

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
    default:;
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
    default:;
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
    return QPoint{
        m_snapX > 1 ? ((point.x() + (m_snapX / 2)) / m_snapX * m_snapX) : point.x(),
        m_snapY > 1 ? ((point.y() + (m_snapY / 2)) / m_snapY * m_snapY) : point.y(),
    };
}
