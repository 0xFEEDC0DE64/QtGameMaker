#pragma once

#include <QWidget>

class QPixmap;

class DrawingCanvasWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float zoom READ zoom WRITE setScale NOTIFY zoomChanged FINAL)
    Q_PROPERTY(QColor leftButtonColor READ leftButtonColor WRITE setLeftButtonColor NOTIFY leftButtonColorChanged FINAL)
    Q_PROPERTY(QColor rightButtonColor READ rightButtonColor WRITE setRightButtonColor NOTIFY rightButtonColorChanged FINAL)

public:
    explicit DrawingCanvasWidget(QWidget *parent = nullptr);

    enum class Mode {
        Draw,
        Spray,
        Erase,
        Pick,
        Line,
        Polygon,
        Rectangle,
        Ellipse,
        RoundedRectangle,
        SelectRegion,
        SelectWand,
        SelectSpray,
        Text,
        Fill,
        Replace,
    };
    Q_ENUM(Mode)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged FINAL)
    Mode mode() const { return m_mode; }
    void setMode(Mode mode);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

    float zoom() const { return m_zoom; }
    void setScale(float zoom);

    QColor leftButtonColor() const { return m_leftButtonColor; }
    void setLeftButtonColor(const QColor &LeftButtonColor);
    QColor rightButtonColor() const { return m_rightButtonColor; }
    void setRightButtonColor(const QColor &rightButtonColor);

signals:
    void changed();

    void cursorMoved(const QPoint &pos);

    void modeChanged(DrawingCanvasWidget::Mode mode);
    void zoomChanged(float zoom);
    void leftButtonColorChanged(const QColor &LeftButtonColor);
    void rightButtonColorChanged(const QColor &rightButtonColor);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setPixel(QPoint pos, bool leftColor);
    void floodFill(QPoint pos, bool leftColor);

    Mode m_mode{Mode::Draw};

    QImage m_image;

    float m_zoom{1.f};

    QColor m_leftButtonColor{Qt::white};
    QColor m_rightButtonColor{Qt::black};

    std::optional<bool> m_currentlyDrawing;
};
