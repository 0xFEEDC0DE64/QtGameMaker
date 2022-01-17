#include "roomeditwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include "editorguiutils.h"
#include "models/projecttreemodel.h"
#include "projectcontainer.h"

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
    if (m_gridEnabled)
        update();
}

void RoomEditWidget::setSnapY(int snapY)
{
    if (m_snapY == snapY)
        return;
    emit snapYChanged(m_snapY = snapY);
    if (m_gridEnabled)
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
    if (m_gridEnabled)
        update();
}

void RoomEditWidget::setSelectedObject(const Object *selectedObject)
{
    if (m_selectedObject == selectedObject)
        return;
    if (m_draggedObject && &m_draggedObject->object.get() == m_selectedObject)
        m_draggedObject = std::nullopt;
    m_selectedObject = selectedObject;
    update();
}

void RoomEditWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QWidget::paintEvent(event);

    if (m_gridEnabled)
    {
        if (!m_gridBrush || m_gridBrush->snapX != m_snapX || m_gridBrush->snapY != m_snapY)
        {
            m_gridBrush = GridBrush {
                .snapX = m_snapX,
                .snapY = m_snapY,
                .brush = makeGridBrush(m_snapX, m_snapY, palette().color(m_gridRole), Qt::transparent)
            };
        }
    }
    else
        m_gridBrush = std::nullopt;

    QPainter painter{this};

    if (m_draggedObject)
    {
        if (m_projectTreeModel)
        {
            QPixmap pixmap;
            const auto &object = m_draggedObject->object.get();
            if (!object.spriteName.isEmpty())
            {
                const auto iter = std::find_if(std::cbegin(m_projectTreeModel->project()->sprites), std::cend(m_projectTreeModel->project()->sprites),
                                               [&object](const auto &sprite){ return object.spriteName == sprite.name; });
                if (iter == std::cend(m_projectTreeModel->project()->sprites))
                    qWarning() << "invalid sprite" << object.spriteName;
                else if (!iter->pixmaps.empty() && !iter->pixmaps.front().isNull())
                    pixmap = iter->pixmaps.front();
            }

            if (pixmap.isNull())
                goto noPixmap;

            painter.drawPixmap(m_draggedObject->pos, std::move(pixmap));
        }
        else
        {
noPixmap:
            painter.drawRect(QRect{m_draggedObject->pos, QSize{64, 64}});
        }
    }

    if (m_gridBrush)
        painter.fillRect(rect(), m_gridBrush->brush);
}

void RoomEditWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    if (m_selectedObject)
    {
        m_draggedObject = DraggedObject {
            .object = *m_selectedObject,
            .pos = event->pos()
        };
        update();
    }
}

void RoomEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if (m_draggedObject)
    {
        m_draggedObject = std::nullopt;
        update();
    }
}

void RoomEditWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    emit cursorMoved(snapPoint(event->pos()));

    if (m_draggedObject)
    {
        const auto newPos = snapPoint(event->pos());
        if (newPos != m_draggedObject->pos)
        {
            m_draggedObject->pos = newPos;
            update();
        }
    }
}

QPoint RoomEditWidget::snapPoint(const QPoint &point) const
{
    return QPoint{
        m_snapX > 1 ? ((point.x() + (m_snapX / 2)) / m_snapX * m_snapX) : point.x(),
        m_snapY > 1 ? ((point.y() + (m_snapY / 2)) / m_snapY * m_snapY) : point.y(),
    };
}
