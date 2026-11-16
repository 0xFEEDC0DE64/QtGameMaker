#pragma once

#include <QTreeView>

class QDrag;

class DraggableTreeView : public QTreeView
{
    Q_OBJECT

public:
    using QTreeView::QTreeView;

    void startDrag(Qt::DropActions supportedActions) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected:
    virtual Qt::DropAction executeDrag(QDrag &drag);
};
