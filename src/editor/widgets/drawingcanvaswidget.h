#pragma once

#include <QWidget>

class QPixmap;

class DrawingCanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingCanvasWidget(QWidget *parent = nullptr);

    QPixmap *pixmap() { return m_pixmap; }
    const QPixmap *pixmap() const { return m_pixmap; }
    void setPixmap(QPixmap &pixmap);

    float scale() const { return m_scale; }
    void setScale(float scale);

signals:
    void scaleChanged(float scale);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPixmap *m_pixmap{};

    float m_scale{4.f};
};
