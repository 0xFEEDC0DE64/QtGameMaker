#pragma once

#include "colorwidget.h"

class CoarseColorWidget : public ColorWidget
{
    Q_OBJECT

public:
    using ColorWidget::ColorWidget;

protected:
    void resizeEvent(QResizeEvent *event) override;
};
