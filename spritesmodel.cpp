#include "spritesmodel.h"

#include <QPixmap>

SpritesModel::SpritesModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

int SpritesModel::rowCount(const QModelIndex &parent) const
{
    return 100;
}

QVariant SpritesModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return tr("image %0").arg(index.row());
    case Qt::DecorationRole:
        QPixmap pixmap{64, 64};
        pixmap.fill(Qt::black);
        return pixmap;
    }

    return {};
}
