#include "roomscene.h"

RoomScene::RoomScene(QObject *parent) :
    QGraphicsScene{parent}
{
    setSceneRect(QRectF{0, 0, 640, 480});
}
