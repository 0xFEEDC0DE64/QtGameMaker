#include "triggersmodel.h"

#include "projectcontainer.h"

TriggersModel::TriggersModel(ProjectContainer &project, QObject *parent) :
    QAbstractListModel{parent},
    m_project{project}
{
}

int TriggersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant TriggersModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
