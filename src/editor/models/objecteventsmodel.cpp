#include "objecteventsmodel.h"

#include <QDebug>
#include <QIcon>

#include "futurecpp.h"

ObjectEventsModel::ObjectEventsModel(Object::events_container_t &events, QObject *parent) :
    QAbstractListModel{parent},
    m_events{events}
{
}

int ObjectEventsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_events.size();
}

QVariant ObjectEventsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)

    if (index.row() < 0 || std::size_t(index.row()) >= m_events.size())
    {
        qWarning() << "row out of bounds" << index.row();
        return {};
    }

    const auto &pair = *std::next(std::cbegin(m_events), index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (pair.first)
        {
        case Object::EventType::Create:  return tr("Create");
        case Object::EventType::Destroy: return tr("Destroy");
        case Object::EventType::Step:    return tr("Step");
        case Object::EventType::Draw:    return tr("Draw");
        default:
            qWarning() << "unknown event type" << std::to_underlying(pair.first);
            return QString::number(std::to_underlying(pair.first));
        }
    case Qt::DecorationRole:
        switch (pair.first)
        {
        case Object::EventType::Create:  return QIcon{":/qtgameengine/icons/event-create.png"};
        case Object::EventType::Destroy: return QIcon{":/qtgameengine/icons/event-destroy.png"};
        case Object::EventType::Step:    return QIcon{":/qtgameengine/icons/event-step.png"};
        case Object::EventType::Draw:    return QIcon{":/qtgameengine/icons/event-draw.png"};
        default:
            qWarning() << "unknown event type" << std::to_underlying(pair.first);
            return {};
        }
    }

    return {};
}

bool ObjectEventsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (row < 0 || std::size_t(row) >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_events.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);
    auto begin = std::next(std::begin(m_events), row);
    auto end = std::next(begin, count);
    m_events.erase(begin, end);
    endRemoveRows();

    return true;
}

bool ObjectEventsModel::addEvent(Object::EventType eventType)
{
    if (m_events.contains(eventType))
    {
        qWarning() << std::to_underlying(eventType) << "duplicate";
        return false;
    }

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempevents = m_events;

    const auto &tempInsertResult = tempevents.insert(std::make_pair(eventType, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempevents), tempInsertResult.first);


    beginInsertRows({}, tempNewRow, tempNewRow);

    const auto &insertResult = m_events.insert(std::make_pair(eventType, ActionsContainer{}));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_events), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool ObjectEventsModel::changeEvent(Object::EventType eventType, Object::EventType newEventType)
{
    const auto iter = m_events.find(eventType);
    if (iter == std::cend(m_events))
    {
        qWarning() << std::to_underlying(eventType) << "not found";
        return false;
    }

    if (eventType == newEventType)
    {
        qWarning() << "same event again";
        return true;
    }

    auto container = std::move(iter->second);

    const auto oldRow = std::distance(std::begin(m_events), iter);

    beginRemoveRows({}, oldRow, oldRow);
    m_events.erase(iter);
    endRemoveRows();

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempevents = m_events;

    const auto &tempInsertResult = tempevents.insert(std::make_pair(newEventType, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempevents), tempInsertResult.first);


    beginInsertRows({}, tempNewRow, tempNewRow);

    const auto &insertResult = m_events.insert(std::make_pair(newEventType, std::move(container)));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_events), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool ObjectEventsModel::removeEvent(Object::EventType eventType)
{
    const auto iter = m_events.find(eventType);
    if (iter == std::cend(m_events))
    {
        qWarning() << std::to_underlying(eventType) << "not found";
        return false;
    }

    const auto row = std::distance(std::begin(m_events), iter);

    beginRemoveRows({}, row, row);
    m_events.erase(iter);
    endRemoveRows();

    return true;
}

std::pair<const Object::EventType, ActionsContainer> *ObjectEventsModel::getEvent(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getEvent(index.row());
}

const std::pair<const Object::EventType, ActionsContainer> *ObjectEventsModel::getEvent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getEvent(index.row());
}

std::pair<const Object::EventType, ActionsContainer> *ObjectEventsModel::getEvent(int row)
{
    if (row < 0 || std::size_t(row) >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::begin(m_events), row);
}

const std::pair<const Object::EventType, ActionsContainer> *ObjectEventsModel::getEvent(int row) const
{
    if (row < 0 || std::size_t(row) >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::cbegin(m_events), row);
}
