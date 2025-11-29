#pragma once

#include <QScrollArea>

class ZoomableScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    using QScrollArea::QScrollArea;

protected:
    void wheelEvent(QWheelEvent *event) override;
};
