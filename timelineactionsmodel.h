#pragma once

#include <QAbstractListModel>

class TimelineActionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimelineActionsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
