#include "objecteventsmodel.h"

ObjectEventsModel::ObjectEventsModel(QObject *parent) :
    QAbstractListModel{parent}
{
}

int ObjectEventsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant ObjectEventsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
