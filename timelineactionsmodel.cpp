#include "timelineactionsmodel.h"

TimelineActionsModel::TimelineActionsModel(QObject *parent) :
    QAbstractListModel{parent}
{
}

int TimelineActionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant TimelineActionsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
