#include "pathpointsmodel.h"

#include <QDebug>

namespace {
enum {
    ColumnPoint,
    ColumnSp,
    NumberOfColumns
};
}

PathPointsModel::PathPointsModel(std::vector<Path::Point> &points, QObject *parent) :
    QAbstractTableModel{parent},
    m_points{points}
{
}

int PathPointsModel::rowCount(const QModelIndex &parent) const
{
    return m_points.size();
}

int PathPointsModel::columnCount(const QModelIndex &parent) const
{
    return NumberOfColumns;
}

QVariant PathPointsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_points.size())
    {
        qWarning() << "invalid row" << index.row();
        return {};
    }

    const auto &point = m_points.at(index.row());

    switch (index.column())
    {
    case ColumnPoint:
        switch (role)
        {
        case Qt::DisplayRole: return tr("(%0,%1)").arg(point.point.x()).arg(point.point.y());
        case Qt::EditRole:    return point.point;
        default:              return {};
        }
    case ColumnSp:
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:    return point.sp;
        default:              return {};
        }
    }

    return {};
}

QVariant PathPointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    switch (section)
    {
    case ColumnPoint: return tr("point");
    case ColumnSp:    return tr("sp");
    }

    return {};
}

Path::Point PathPointsModel::getPoint(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index" << index;
        return {};
    }

    if (index.row() < 0 || index.row() >= m_points.size())
    {
        qWarning() << "unexpected row" << index.row();
        return {};
    }

    return m_points.at(index.row());
}

void PathPointsModel::pointInserted(std::size_t index)
{
    emit beginInsertRows({}, index, index);
    emit endInsertRows();
}

void PathPointsModel::pointMoved(std::size_t index)
{
    emit dataChanged(this->index(index, 0), this->index(index, 0), {Qt::DisplayRole, Qt::EditRole});
}
