#pragma once

#include <QToolButton>
#include <QPoint>

class ActionDragWidget : public QToolButton
{
    Q_OBJECT

public:
    explicit ActionDragWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_dragStartPosition;
};
