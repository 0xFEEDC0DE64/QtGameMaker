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
    Q_UNUSED(parent)
    return m_points.size();
}

int PathPointsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return NumberOfColumns;
}

QVariant PathPointsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || std::size_t(index.row()) >= m_points.size())
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

bool PathPointsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (row < 0 || std::size_t(row) > m_points.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    beginInsertRows({}, row, row + count - 1);
    for (size_t i = 0; i < std::size_t(count); i++)
        m_points.emplace(std::next(std::begin(m_points), row + i));
    endInsertRows();

    return true;
}

bool PathPointsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (row < 0 || std::size_t(row) >= m_points.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_points.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);
    auto begin = std::next(std::begin(m_points), row);
    auto end = std::next(begin, count);
    m_points.erase(begin, end);
    endRemoveRows();

    return true;
}

std::optional<Path::Point> PathPointsModel::getPoint(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index" << index;
        return {};
    }

    return getPoint(index.row());
}

std::optional<Path::Point> PathPointsModel::getPoint(int row) const
{
    if (row < 0 || std::size_t(row) >= m_points.size())
    {
        qWarning() << "unexpected row" << row;
        return std::nullopt;
    }

    return m_points.at(row);
}

bool PathPointsModel::insertPoint(std::size_t index, Path::Point point)
{
    if (index > m_points.size())
    {
        qWarning() << "unexpected index" << index;
        return false;
    }

    beginInsertRows({}, index, index);
    m_points.emplace(std::next(std::begin(m_points), index), point);
    endInsertRows();

    return true;
}

bool PathPointsModel::updatePoint(std::size_t index, Path::Point point)
{
    if (index >= m_points.size())
    {
        qWarning() << "unexpected index" << index;
        return false;
    }

    m_points.at(index) = point;
    const auto modelIndex = this->index(index, 0);
    emit dataChanged(modelIndex, modelIndex, { Qt::DisplayRole, Qt::EditRole });

    return true;
}

bool PathPointsModel::removePoint(std::size_t index)
{
    if (index >= m_points.size())
    {
        qWarning() << "unexpected index" << index;
        return false;
    }

    beginRemoveRows({}, index, index);
    m_points.erase(std::next(std::begin(m_points), index));
    endRemoveRows();

    return true;
}

void PathPointsModel::pointInserted(std::size_t index)
{
    beginInsertRows({}, index, index);
    endInsertRows();
}

void PathPointsModel::pointMoved(std::size_t index)
{
    const auto modelIndex = this->index(index, 0);
    emit dataChanged(modelIndex, modelIndex, {Qt::DisplayRole, Qt::EditRole});
}
