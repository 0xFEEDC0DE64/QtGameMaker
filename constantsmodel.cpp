#include "constantsmodel.h"

#include "projectcontainer.h"

namespace {
enum {
    ColumnName,
    ColumnValue,
    NumberOfColumns
};
}

ConstantsModel::ConstantsModel(ProjectContainer &project, QObject *parent) :
    QAbstractTableModel{parent},
    m_project{project}
{
}

int ConstantsModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int ConstantsModel::columnCount(const QModelIndex &parent) const
{
    return NumberOfColumns;
}

QVariant ConstantsModel::data(const QModelIndex &index, int role) const
{
    return {};
}

QVariant ConstantsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    switch (section)
    {
    case ColumnName:  return tr("Name");
    case ColumnValue: return tr("Value");
    }

    return {};
}
