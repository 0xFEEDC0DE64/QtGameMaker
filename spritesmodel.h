#pragma once

#include <QAbstractListModel>

class SpritesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SpritesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
