#pragma once

#include <QTreeView>

class DraggableTreeView : public QTreeView
{
    Q_OBJECT

public:
    using QTreeView::QTreeView;

    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};
