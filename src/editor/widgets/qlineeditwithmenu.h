#pragma once

#include <QLineEdit>

class QMenu;

class QLineEditWithMenu : public QLineEdit
{
    Q_OBJECT

public:
    using QLineEdit::QLineEdit;

    QMenu *menu() { return m_menu; }
    const QMenu *menu() const { return m_menu; }
    void setMenu(QMenu *menu) { m_menu = menu; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMenu *m_menu{};
};
