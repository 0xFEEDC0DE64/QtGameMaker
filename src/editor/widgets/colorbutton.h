#pragma once

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true FINAL)

public:
    explicit ColorButton(QWidget *parent = nullptr);
    explicit ColorButton(const QString &text, QWidget *parent = nullptr);
    ColorButton(const QIcon& icon, const QString &text, QWidget *parent = nullptr);

    QColor color() const { return m_color; }

public slots:
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

private slots:
    void selectColor();

private:
    QColor m_color;
};
