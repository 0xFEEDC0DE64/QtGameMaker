#pragma once

#include <QGraphicsScene>

class RoomScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit RoomScene(QObject *parent = nullptr);
};
