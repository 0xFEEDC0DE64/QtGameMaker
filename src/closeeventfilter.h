#pragma once

#include <QObject>

class CloseEventFilter : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

signals:
    void closeEventReceived();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
