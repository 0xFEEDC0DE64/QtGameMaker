#include "finecolorwidget.h"

void FineColorWidget::resizeEvent(QResizeEvent *event)
{
    ColorWidget::resizeEvent(event);

    int w = width() - frameWidth() * 2;
    int h = height() - frameWidth() * 2;

    m_image = QImage{w, h, QImage::Format_RGB32};

    for (int y = 0; y < h; y++)
    {
        uint *pixel = (uint *)m_image.scanLine(y);
        float hue = float(360 - y * 360 / (h - 1));

        QColor pure;
        pure.setHsv(hue, 255, 255);

        for (int x = 0; x < w; x++, pixel++)
        {
            float t = float(x) / float(w - 1);   // 0..1 across width
            QColor c;

            if (t < 0.5f)
            {
                float k = t / 0.5f;  // 0..1 across halve
                c.setRgb(
                    pure.red()   * k,
                    pure.green() * k,
                    pure.blue()  * k
                    );
            }
            else
            {
                float k = (t - 0.5f) / 0.5f;  // 0..1 across halve
                c.setRgb(
                    pure.red()   + (255 - pure.red())   * k,
                    pure.green() + (255 - pure.green()) * k,
                    pure.blue()  + (255 - pure.blue())  * k
                    );
            }

            *pixel = c.rgb();
        }
    }

    m_pixmap = QPixmap::fromImage(m_image);
}
