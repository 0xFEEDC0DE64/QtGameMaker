#include "draggabletreeview.h"

#include <QDrag>
#include <QMimeData>
#include <QPersistentModelIndex>
#include <QItemSelectionModel>

#include <vector>

void DraggableTreeView::startDrag(Qt::DropActions supportedActions)
{
    if (!(supportedActions & Qt::MoveAction) || !model())
        return;

    QModelIndexList draggable;
    for (const QModelIndex &index : selectedIndexes())
        if (index.flags() & Qt::ItemIsDragEnabled)
            draggable.push_back(index);
    if (draggable.isEmpty())
        return;
    const QPersistentModelIndex moved{draggable.front()};

    QMimeData *data = model()->mimeData(draggable);
    if (!data)
        return;

    // dropMimeData() moves the node with beginMoveRows()/endMoveRows(). The
    // QAbstractItemView implementation of startDrag() removes the selected
    // source row after QDrag::exec() returns MoveAction, which would delete
    // the resource that has already been moved into its destination folder.
    QDrag drag(this);
    drag.setMimeData(data);
    if (executeDrag(drag) == Qt::MoveAction && moved.isValid())
    {
        std::vector<QModelIndex> ancestors;
        for (QModelIndex ancestor = moved.parent(); ancestor.isValid(); ancestor = ancestor.parent())
            ancestors.push_back(ancestor);
        for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it)
            expand(*it);
        selectionModel()->setCurrentIndex(moved, QItemSelectionModel::ClearAndSelect);
        scrollTo(moved, QAbstractItemView::PositionAtCenter);
        setFocus(Qt::OtherFocusReason);
    }
}

Qt::DropAction DraggableTreeView::executeDrag(QDrag &drag)
{
    return drag.exec(Qt::MoveAction, Qt::MoveAction);
}

void DraggableTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    setDropIndicatorShown(true);
    QTreeView::dragMoveEvent(event);
}

void DraggableTreeView::dropEvent(QDropEvent *event)
{
    QTreeView::dropEvent(event);
}

void DraggableTreeView::paintEvent(QPaintEvent *event)
{
    DropIndicatorPosition position = dropIndicatorPosition();
    setDropIndicatorShown(position == QAbstractItemView::BelowItem ||
                          position == QAbstractItemView::AboveItem ||
                          position == QAbstractItemView::OnItem);

    QTreeView::paintEvent(event);

    setDropIndicatorShown( true );
}
