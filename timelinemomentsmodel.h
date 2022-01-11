#pragma once

#include <QAbstractListModel>

class TimelineMomentsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimelineMomentsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
