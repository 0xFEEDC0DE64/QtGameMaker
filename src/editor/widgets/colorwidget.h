#pragma once

#include <QFrame>
#include <QImage>
#include <QPixmap>

#include <optional>

class ColorWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ColorWidget(QWidget *parent = nullptr);
    ~ColorWidget() override;

signals:
    void colorLeftClicked(const QColor &color);
    void colorRightClicked(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QImage m_image;
    QPixmap m_pixmap;

private:
    void pickPixel(const QPoint &pos, bool left);

    std::optional<bool> m_currentlyPicking;
};
