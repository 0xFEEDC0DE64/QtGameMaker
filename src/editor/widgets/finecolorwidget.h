#pragma once

#include "colorwidget.h"

class FineColorWidget : public ColorWidget
{
    Q_OBJECT

public:
    using ColorWidget::ColorWidget;

protected:
    void resizeEvent(QResizeEvent *event) override;
};
