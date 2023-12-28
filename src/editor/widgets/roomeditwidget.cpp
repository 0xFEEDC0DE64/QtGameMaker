#include "roomeditwidget.h"

#include <iterator>

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

void RoomEditWidget::setObjects(std::vector<Room::Object> *objects)
{
    m_objects = objects;
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

    paintObjects(painter);
    paintDraggedObject(painter);

    if (m_gridBrush)
        painter.fillRect(rect(), m_gridBrush->brush);
}

void RoomEditWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    switch (event->button())
    {
    case Qt::MouseButton::LeftButton:
        if (m_selectedObject)
        {
            m_draggedObject = DraggedObject {
                .object = *m_selectedObject,
                .pos = snapPoint(event->pos())
            };
            update();
        }
        break;
    case Qt::MouseButton::RightButton:
    {
        if (m_projectTreeModel && m_objects)
        {
            auto iter = std::find_if(std::rbegin(*m_objects), std::rend(*m_objects),
                                     [&](const Room::Object &obj){

                const auto &objectName = obj.objectName;

                const auto &objects = m_projectTreeModel->project()->objects;
                const auto iter = std::find_if(std::cbegin(objects), std::cend(objects),
                                               [&](const Object &obj){ return obj.name == objectName; });
                if (iter == std::cend(objects))
                {
                    qWarning() << "object" << obj.objectName << "not found";
                    return false;
                }
                else
                {
                    const Object &object = *iter;
                    const auto &spriteName = object.spriteName;

                    const auto &sprites = m_projectTreeModel->project()->sprites;
                    const auto iter = std::find_if(std::cbegin(sprites), std::cend(sprites),
                                                   [&](const Sprite &sprite){ return sprite.name == spriteName; });
                    if (iter == std::cend(sprites))
                    {
                        qWarning() << "sprite" << spriteName << "not found";
                        return false;
                    }
                    else
                    {
                        const Sprite &sprite = *iter;

                        if (sprite.pixmaps.empty())
                            return false;

                        return QRect(obj.pos, sprite.pixmaps.front().size()).contains(event->pos());
                    }
                }
            });
            if (iter != std::rend(*m_objects))
            {
                m_objects->erase(std::next(iter).base());

                emit changed();

                update();
            }
        }
        break;
    }
    default:;
    }
}

void RoomEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    switch (event->button())
    {
    case Qt::MouseButton::LeftButton:
        if (m_draggedObject)
        {
            if (m_objects)
            {
                m_objects->emplace_back(Room::Object{
                    .objectName = m_draggedObject->object.get().name,
                    .pos = snapPoint(event->pos())
                });

                emit changed();
            }

            m_draggedObject = std::nullopt;
            update();
        }
    default:;
    }
}

void RoomEditWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    const auto snappedPos = snapPoint(event->pos());

    emit cursorMoved(snappedPos);

    if (m_draggedObject)
        if (snappedPos != m_draggedObject->pos)
        {
            m_draggedObject->pos = snappedPos;
            update();
        }
}

QPoint RoomEditWidget::snapPoint(const QPoint &point) const
{
    return QPoint{
        m_snapX > 1 ? ((point.x() + (m_snapX / 2)) / m_snapX * m_snapX) : point.x(),
        m_snapY > 1 ? ((point.y() + (m_snapY / 2)) / m_snapY * m_snapY) : point.y(),
    };
}

void RoomEditWidget::paintObjects(QPainter &painter)
{
    if (!m_objects)
        return;

    for (const Room::Object &object : *m_objects)
    {
        if (m_projectTreeModel)
        {
            const auto iter = std::find_if(std::cbegin(m_projectTreeModel->project()->objects), std::cend(m_projectTreeModel->project()->objects),
                                           [&object](const auto &obj){ return object.objectName == obj.name; });
            if (iter == std::cend(m_projectTreeModel->project()->objects))
            {
                qWarning() << "invalid object" << object.objectName;
                goto noPixmap;
            }

            if (iter->spriteName.isEmpty())
                goto noPixmap;

            {
                const auto iter2 = std::find_if(std::cbegin(m_projectTreeModel->project()->sprites), std::cend(m_projectTreeModel->project()->sprites),
                                               [&object=*iter](const auto &sprite){ return object.spriteName == sprite.name; });

                if (iter2 == std::cend(m_projectTreeModel->project()->sprites))
                {
                    qWarning() << "invalid sprite" << iter->spriteName;
                    goto noPixmap;
                }

                if (iter2->pixmaps.empty() || iter2->pixmaps.front().isNull())
                    goto noPixmap;

                painter.drawPixmap(object.pos, iter2->pixmaps.front());
            }
        }
        else
        {
noPixmap:
            painter.drawRect(QRect{object.pos, QSize{64, 64}});
        }
    }
}

void RoomEditWidget::paintDraggedObject(QPainter &painter)
{
    if (!m_draggedObject)
        return;

    if (m_projectTreeModel)
    {
        const auto &object = m_draggedObject->object.get();

        if (object.spriteName.isEmpty())
            goto noPixmap;

        {
            const auto iter = std::find_if(std::cbegin(m_projectTreeModel->project()->sprites), std::cend(m_projectTreeModel->project()->sprites),
                                           [&object](const auto &sprite){ return object.spriteName == sprite.name; });
            if (iter == std::cend(m_projectTreeModel->project()->sprites))
            {
                qWarning() << "invalid sprite" << object.spriteName;
                goto noPixmap;
            }

            if (iter->pixmaps.empty() || iter->pixmaps.front().isNull())
                goto noPixmap;

            painter.drawPixmap(m_draggedObject->pos, iter->pixmaps.front());
        }
    }
    else
    {
noPixmap:
        painter.drawRect(QRect{m_draggedObject->pos, QSize{64, 64}});
    }
}
