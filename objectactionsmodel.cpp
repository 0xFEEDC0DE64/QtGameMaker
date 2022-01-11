#include "objectactionsmodel.h"

ObjectActionsModel::ObjectActionsModel(QObject *parent) :
    QAbstractListModel{parent}
{
}

int ObjectActionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant ObjectActionsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
