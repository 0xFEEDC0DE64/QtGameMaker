#pragma once

#include <QScrollArea>

class QMenu;

class QScrollAreaWithMenu : public QScrollArea
{
    Q_OBJECT

public:
    using QScrollArea::QScrollArea;

    QMenu *menu() { return m_menu; }
    const QMenu *menu() const { return m_menu; }
    void setMenu(QMenu *menu) { m_menu = menu; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMenu *m_menu{};
};
