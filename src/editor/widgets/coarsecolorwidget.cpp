#include "coarsecolorwidget.h"

#include <QPainter>

void CoarseColorWidget::resizeEvent(QResizeEvent *event)
{
    ColorWidget::resizeEvent(event);

    int w = width()  - frameWidth() * 2;
    int h = height() - frameWidth() * 2;

    // dynamic columns
    constexpr int MIN_COL_WIDTH  = 16;
    constexpr int MAX_COL_WIDTH  = 32;

    int minCols = std::max(1, w / MAX_COL_WIDTH);
    int maxCols = std::max(1, w / MIN_COL_WIDTH);
    int columns = std::clamp(maxCols, minCols, maxCols);

    // dynamic rows
    constexpr int MIN_ROW_HEIGHT = 16;
    constexpr int MAX_ROW_HEIGHT = 32;

    int minRows = std::max(1, h / MAX_ROW_HEIGHT);
    int maxRows = std::max(1, h / MIN_ROW_HEIGHT);
    int rows = std::clamp(maxRows, minRows, maxRows);

    m_image = QImage(w, h, QImage::Format_RGB32);

    {
        QPainter p(&m_image);
        p.setPen(Qt::NoPen);

        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < columns; ++c)
            {
                // fractional geometry (smooth scaling)
                int x0 = (c    * w) / columns;
                int x1 = ((c+1) * w) / columns;
                int y0 = (r    * h) / rows;
                int y1 = ((r+1) * h) / rows;

                int cw = x1 - x0;
                int rh = y1 - y0;

                // color logic
                QColor color;
                float hue = float(c) * 360.f / float(columns);

                // bottom row = grayscale, but now rows are dynamic
                if (r == rows - 1)
                {
                    int v = int(c * 255.f / std::max(1, columns - 1));
                    color = QColor(v, v, v);
                }
                else
                {
                    // spread the "classic5 rows" over whatever number of rows we have
                    // We interpolate S and V vertically.
                    float t = float(r) / float(rows - 1); // 0..1, except grayscale row

                    // Windows-like palette interpolation:
                    // r=0 (top):   S=255 V=140
                    // r=1:         S=255 V=200
                    // r=2:         S=255 V=255
                    // r=3:         S=100 V=255
                    // r=4:         S= 40 V=255
                    // r=5 bottom:  grayscale row (handled above)

                    // Predefined control points
                    constexpr struct { int s, v; } ctrl[] = {
                        {255, 140},
                        {255, 200},
                        {255, 255},
                        {100, 255},
                        { 40, 255}
                    };

                    int ctrlRows = 5;

                    float pos = t * (ctrlRows - 1);
                    int i = int(pos);
                    float f = pos - i;

                    if (i >= ctrlRows - 1) {
                        i = ctrlRows - 2;
                        f = 1.f;
                    }

                    int S = int(ctrl[i].s * (1.f - f) + ctrl[i + 1].s * f);
                    int V = int(ctrl[i].v * (1.f - f) + ctrl[i + 1].v * f);

                    color.setHsv(int(hue), S, V);
                }

                p.setBrush(color);
                p.drawRect(x0, y0, cw, rh);
            }
    }

    m_pixmap = QPixmap::fromImage(m_image);
}
