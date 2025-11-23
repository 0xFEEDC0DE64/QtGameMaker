#pragma once

#include <QToolButton>
#include <QPoint>

#include "projectcontainer.h"

class ActionDragWidget : public QToolButton
{
    Q_OBJECT

public:
    using QToolButton::QToolButton;

    const Action &action() const { return m_action; }
    void setAction(Action &&action) { m_action = std::move(action); }
    void setAction(const Action &action) { m_action = action; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_dragStartPosition;

    Action m_action;
};
