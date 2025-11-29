#pragma once

#include <QWidget>

class QPixmap;

class DrawingCanvasWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)

    Q_PROPERTY(QColor leftButtonColor READ leftButtonColor WRITE setLeftButtonColor NOTIFY leftButtonColorChanged FINAL)
    Q_PROPERTY(QColor rightButtonColor READ rightButtonColor WRITE setRightButtonColor NOTIFY rightButtonColorChanged FINAL)

public:
    using QWidget::QWidget;

    QPixmap *pixmap() { return m_pixmap; }
    const QPixmap *pixmap() const { return m_pixmap; }
    void setPixmap(QPixmap &pixmap);

    float scale() const { return m_scale; }
    void setScale(float scale);

    QColor leftButtonColor() const { return m_leftButtonColor; }
    void setLeftButtonColor(const QColor &LeftButtonColor);
    QColor rightButtonColor() const { return m_rightButtonColor; }
    void setRightButtonColor(const QColor &rightButtonColor);

signals:
    void changed();

    void scaleChanged(float scale);
    void leftButtonColorChanged(const QColor &LeftButtonColor);
    void rightButtonColorChanged(const QColor &rightButtonColor);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setPixel(QPoint pos, bool leftColor);

    QPixmap *m_pixmap{};

    float m_scale{1.f};

    QColor m_leftButtonColor{Qt::white};
    QColor m_rightButtonColor{Qt::black};

    std::optional<bool> m_currentlyDrawing;
};
