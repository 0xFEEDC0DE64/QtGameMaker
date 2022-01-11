#pragma once

#include <QAbstractListModel>

class ObjectActionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ObjectActionsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
