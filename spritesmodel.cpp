#include "spritesmodel.h"

#include <QPixmap>
#include <QDebug>

SpritesModel::SpritesModel(const std::vector<QPixmap> &pixmaps, QObject *parent) :
    QAbstractListModel{parent},
    m_pixmaps{pixmaps}
{
}

int SpritesModel::rowCount(const QModelIndex &parent) const
{
    return m_pixmaps.size();
}

QVariant SpritesModel::data(const QModelIndex &index, int role) const
{
    if (index.column() != 0)
        return {};

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return tr("image %0").arg(index.row());
    case Qt::DecorationRole:
        if (index.row() < 0 || index.row() >= m_pixmaps.size())
        {
            qWarning() << "invalid row" << index.row();
            return {};
        }

        const auto &pixmap = m_pixmaps.at(index.row());
        if (pixmap.isNull())
        {
            QPixmap pixmap{32, 32};
            pixmap.fill(Qt::white);
            return pixmap;
        }
        return pixmap;
    }

    return {};
}

const QPixmap &SpritesModel::pixmap(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= m_pixmaps.size())
        qFatal("index out of range");

    return m_pixmaps.at(index.row());
}
