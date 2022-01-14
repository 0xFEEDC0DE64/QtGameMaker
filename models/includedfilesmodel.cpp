#include "includedfilesmodel.h"

#include "projectcontainer.h"

IncludedFilesModel::IncludedFilesModel(ProjectContainer &project, QObject *parent) :
    QAbstractListModel{parent},
    m_project{project}
{
}

int IncludedFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant IncludedFilesModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
}
