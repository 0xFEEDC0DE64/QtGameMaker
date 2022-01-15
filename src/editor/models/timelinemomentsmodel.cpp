#include "timelinemomentsmodel.h"

#include <QDebug>

TimelineMomentsModel::TimelineMomentsModel(TimeLine::moments_container_t &moments, QObject *parent) :
    QAbstractListModel{parent},
    m_moments{moments}
{
}

int TimelineMomentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_moments.size();
}

QVariant TimelineMomentsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)

    if (index.row() < 0 || std::size_t(index.row()) >= m_moments.size())
    {
        qWarning() << "row out of bounds" << index.row();
        return {};
    }

    const auto &pair = *std::next(std::cbegin(m_moments), index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return tr("Step %0").arg(pair.first);
    default:
        return {};
    }

    return {};
}

bool TimelineMomentsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (row < 0 || std::size_t(row) >= m_moments.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_moments.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);
    auto begin = std::next(std::begin(m_moments), row);
    auto end = std::next(begin, count);
    m_moments.erase(begin, end);
    endRemoveRows();

    return true;
}

bool TimelineMomentsModel::addMoment(TimeLine::moment_key_t moment)
{
    if (m_moments.contains(moment))
    {
        qWarning() << moment << "duplicate";
        return false;
    }

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempMoments = m_moments;

    const auto &tempInsertResult = tempMoments.insert(std::make_pair(moment, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempMoments), tempInsertResult.first);


    beginInsertRows({}, tempNewRow, tempNewRow);

    const auto &insertResult = m_moments.insert(std::make_pair(moment, ActionsContainer{}));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_moments), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool TimelineMomentsModel::changeMoment(TimeLine::moment_key_t moment, TimeLine::moment_key_t newMoment)
{
    const auto iter = m_moments.find(moment);
    if (iter == std::cend(m_moments))
    {
        qWarning() << moment << "not found";
        return false;
    }

    if (moment == newMoment)
    {
        qWarning() << "same moment again";
        return true;
    }

    auto container = std::move(iter->second);

    const auto oldRow = std::distance(std::begin(m_moments), iter);

    beginRemoveRows({}, oldRow, oldRow);
    m_moments.erase(iter);
    endRemoveRows();

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempMoments = m_moments;

    const auto &tempInsertResult = tempMoments.insert(std::make_pair(newMoment, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempMoments), tempInsertResult.first);


    beginInsertRows({}, tempNewRow, tempNewRow);

    const auto &insertResult = m_moments.insert(std::make_pair(newMoment, std::move(container)));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_moments), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool TimelineMomentsModel::removeMoment(TimeLine::moment_key_t moment)
{
    const auto iter = m_moments.find(moment);
    if (iter == std::cend(m_moments))
    {
        qWarning() << moment << "not found";
        return false;
    }

    const auto row = std::distance(std::begin(m_moments), iter);

    beginRemoveRows({}, row, row);
    m_moments.erase(iter);
    endRemoveRows();

    return true;
}

bool TimelineMomentsModel::clearMoments()
{
    if (m_moments.empty())
    {
        qWarning() << "moments already empty";
        return true;
    }

    beginRemoveRows({}, 0, m_moments.size() - 1);
    m_moments.clear();
    endRemoveRows();

    return true;
}

std::pair<const TimeLine::moment_key_t, ActionsContainer> *TimelineMomentsModel::getMoment(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getMoment(index.row());
}

const std::pair<const TimeLine::moment_key_t, ActionsContainer> *TimelineMomentsModel::getMoment(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getMoment(index.row());
}

std::pair<const TimeLine::moment_key_t, ActionsContainer> *TimelineMomentsModel::getMoment(int row)
{
    if (row < 0 || std::size_t(row) >= m_moments.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::begin(m_moments), row);
}

const std::pair<const TimeLine::moment_key_t, ActionsContainer> *TimelineMomentsModel::getMoment(int row) const
{
    if (row < 0 || std::size_t(row) >= m_moments.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::cbegin(m_moments), row);
}
