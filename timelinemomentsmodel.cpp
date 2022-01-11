#include "timelinemomentsmodel.h"

TimelineMomentsModel::TimelineMomentsModel(QObject *parent) :
    QAbstractListModel{parent}
{
}

int TimelineMomentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant TimelineMomentsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
