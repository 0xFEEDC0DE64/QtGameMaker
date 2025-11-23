#include "draggabletreeview.h"

void DraggableTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    setDropIndicatorShown(true);
    //qDebug() << QStringList{"OnItem","AboveItem","BelowItem","OnViewport"}[dropIndicatorPosition()];
    QTreeView::dragMoveEvent(event);
}

void DraggableTreeView::dropEvent(QDropEvent *event)
{
    bool dropOK = false;
    DropIndicatorPosition dropIndicator = dropIndicatorPosition();

    switch (dropIndicator)
    {
    case QAbstractItemView::AboveItem:
        dropOK = true;
        break;
    case QAbstractItemView::BelowItem:
        dropOK = true;
        break;
    case QAbstractItemView::OnItem:
        dropOK = false;
        break;
    case QAbstractItemView::OnViewport:
        dropOK = false;
        break;
    }
    if(dropOK)
    {
        // Here, you need to manage yourself the case of dropping an item
    }
    setDropIndicatorShown(false); // hide the drop indicator once the drop is done
    QTreeView::dropEvent(event);
}

void DraggableTreeView::paintEvent(QPaintEvent *event)
{
    DropIndicatorPosition position = dropIndicatorPosition();
    setDropIndicatorShown( position == QAbstractItemView::BelowItem || position == QAbstractItemView::AboveItem );

    QTreeView::paintEvent(event);

    setDropIndicatorShown( true );
}
