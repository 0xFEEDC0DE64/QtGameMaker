#pragma once

#include <QAbstractListModel>
#include <QPixmap>

#include <vector>

class SpritesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SpritesModel(const std::vector<QPixmap> &pixmaps, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    const QPixmap &pixmap(const QModelIndex &index) const;

    using QAbstractListModel::beginResetModel;
    using QAbstractListModel::endResetModel;

    using QAbstractListModel::beginInsertRows;
    using QAbstractListModel::endInsertRows;

private:
    const std::vector<QPixmap> &m_pixmaps;
};
