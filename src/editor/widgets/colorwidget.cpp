#include "colorwidget.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

ColorWidget::ColorWidget(QWidget *parent)
    : QFrame{parent}
{
    setCol(150, 255);
    setAttribute(Qt::WA_NoSystemBackground);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
}

ColorWidget::~ColorWidget() = default;

void ColorWidget::setCol(int h, int s)
{
    int nhue = qMin(qMax(0,h), 359);
    int nsat = qMin(qMax(0,s), 255);
    if (nhue == m_hue && nsat == m_sat)
        return;
    QRect r(colPt(), QSize(20,20));
    m_hue = nhue;
    m_sat = nsat;
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);
//    update(r);
    repaint(r);
}

void ColorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    drawFrame(&p);
    QRect r = contentsRect();
    p.drawPixmap(r.topLeft(), m_pixmap);
}

void ColorWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    int w = width() - frameWidth() * 2;
    int h = height() - frameWidth() * 2;
    QImage img(w, h, QImage::Format_RGB32);
    int x, y;
    uint *pixel = (uint *) img.scanLine(0);
    for (y = 0; y < h; y++)
    {
        const uint *end = pixel + w;
        x = 0;
        while (pixel < end) {
            QPoint p(x, y);
            QColor c;
            c.setHsv(huePt(p), satPt(p), 200);
            *pixel = c.rgb();
            ++pixel;
            ++x;
        }
    }
    m_pixmap = QPixmap::fromImage(img);
}

QPoint ColorWidget::colPt()
{
    QRect r = contentsRect();
    return QPoint((360 - m_hue) * (r.width() - 1) / 360, (255 - m_sat) * (r.height() - 1) / 255);
}

int ColorWidget::huePt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 360 - pt.x() * 360 / (r.width() - 1);
}

int ColorWidget::satPt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 255 - pt.y() * 255 / (r.height() - 1);
}

void ColorWidget::setCol(const QPoint &pt)
{
    setCol(huePt(pt), satPt(pt));
}
