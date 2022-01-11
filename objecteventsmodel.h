#pragma once

#include <QAbstractListModel>

class ObjectEventsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ObjectEventsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
