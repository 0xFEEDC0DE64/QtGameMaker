#pragma once

#include <QObject>

class CloseEventFilter : public QObject
{
    Q_OBJECT

public:
    CloseEventFilter(QObject *parent = nullptr);

signals:
    void closeEventReceived();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
