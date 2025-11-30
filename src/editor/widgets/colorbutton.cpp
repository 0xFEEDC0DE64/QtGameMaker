#include "colorbutton.h"

#include <QColorDialog>

ColorButton::ColorButton(QWidget *parent) :
    QPushButton{parent}
{
    connect(this, &QAbstractButton::pressed,
            this, &ColorButton::selectColor);
}

ColorButton::ColorButton(const QString &text, QWidget *parent) :
    QPushButton{text, parent}
{
    connect(this, &QAbstractButton::pressed,
            this, &ColorButton::selectColor);
}

ColorButton::ColorButton(const QIcon &icon, const QString &text, QWidget *parent) :
    QPushButton{icon, text, parent}
{
    connect(this, &QAbstractButton::pressed,
            this, &ColorButton::selectColor);
}

void ColorButton::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    auto pal = palette();
    pal.setBrush(backgroundRole(), color);
    setPalette(pal);

    emit colorChanged(m_color = color);
}

void ColorButton::selectColor()
{
    QColorDialog dialog{m_color, this};
    if (dialog.exec() == QDialog::Accepted)
        setColor(dialog.selectedColor());
}
