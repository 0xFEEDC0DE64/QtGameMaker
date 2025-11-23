#pragma once

#include <QFrame>
#include <QPixmap>

class ColorWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ColorWidget(QWidget *parent = nullptr);
    ~ColorWidget() override;

public slots:
    void setCol(int h, int s);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPoint colPt();
    int huePt(const QPoint &pt);
    int satPt(const QPoint &pt);
    void setCol(const QPoint &pt);

    int m_hue{};
    int m_sat{};

    QPixmap m_pixmap;
};
