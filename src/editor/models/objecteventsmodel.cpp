#include "objecteventsmodel.h"

#include <QDebug>
#include <QIcon>

#include "futurecpp.h"

ObjectEventsModel::ObjectEventsModel(Object::events_container_t &events, Object::collision_events_container_t &collisionEvents,
                                     QObject *parent) :
    QAbstractListModel{parent},
    m_events{events},
    m_collisionEvents{collisionEvents}
{
}

int ObjectEventsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_events.size() + m_collisionEvents.size();
}

QVariant ObjectEventsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)

    if (index.row() < 0)
    {
        qWarning() << "row out of bounds" << index.row();
        return {};
    }
    else if (std::size_t(index.row()) < m_events.size())
    {
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
    }
    else if (std::size_t(index.row()) < m_collisionEvents.size() + m_events.size())
    {
        const auto &pair = *std::next(std::cbegin(m_collisionEvents), index.row() - m_events.size());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return pair.first;
        case Qt::DecorationRole:
            return QIcon{":/qtgameengine/icons/event-collision.png"};
        }
    }
    else
    {
        qWarning() << "row out of bounds" << index.row();
        return {};
    }

    return {};
}

bool ObjectEventsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (row < 0 || std::size_t(row) >= m_events.size() + m_collisionEvents.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_events.size() + m_collisionEvents.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);

    if (std::size_t(row) < m_events.size())
    {
        const auto remaining = m_events.size() - row;
        const auto begin = std::next(std::begin(m_events), row);
        const auto end = std::next(begin, std::min<std::size_t>(count, remaining));
        m_events.erase(begin, end);

        if (std::size_t(count) > remaining)
        {
            count -= remaining;

            const auto begin = std::begin(m_collisionEvents);
            const auto end = std::next(begin, count);
            m_collisionEvents.erase(begin, end);
        }
    }
    else if (std::size_t(row) < m_events.size() + m_collisionEvents.size())
    {
        qDebug() << row - m_events.size();
        qDebug() << count;
        qDebug() << m_collisionEvents.size();

        const auto begin = std::next(std::begin(m_collisionEvents), row - m_events.size());
        const auto end = std::next(begin, count);
        m_collisionEvents.erase(begin, end);
    }

    endRemoveRows();

    return true;
}

bool ObjectEventsModel::addEvent(std::variant<Object::EventType, QString> eventType)
{
    if (const auto &type = std::get_if<Object::EventType>(&eventType))
        return addEvent(*type);
    if (const auto &object = std::get_if<QString>(&eventType))
        return addEvent(*object);
    qCritical() << "not implemented";
    return false;
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

bool ObjectEventsModel::addEvent(const QString &object)
{
    if (m_collisionEvents.contains(object))
    {
        qWarning() << object << "duplicate";
        return false;
    }

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempevents = m_collisionEvents;

    const auto &tempInsertResult = tempevents.insert(std::make_pair(object, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempevents), tempInsertResult.first);


    beginInsertRows({}, tempNewRow + m_events.size(), tempNewRow + m_events.size());

    const auto &insertResult = m_collisionEvents.insert(std::make_pair(object, ActionsContainer{}));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_collisionEvents), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool ObjectEventsModel::changeEvent(std::variant<Object::EventType, QString> eventType, std::variant<Object::EventType, QString> newEventType)
{
    if (const auto &type = std::get_if<Object::EventType>(&eventType))
    {
        if (const auto &newType = std::get_if<Object::EventType>(&newEventType))
            return changeEvent(*type, *newType);
        if (const auto &newObject = std::get_if<QString>(&newEventType))
        {
            //return changeEvent(*type, *newObject);
        }
        qCritical() << "not implemented";
        return false;
    }
    if (const auto &object = std::get_if<QString>(&eventType))
    {
        if (const auto &newType = std::get_if<Object::EventType>(&newEventType))
        {
            //return changeEvent(*object, *newType);
        }
        if (const auto &newObject = std::get_if<QString>(&newEventType))
            return changeEvent(*object, *newObject);
        qCritical() << "not implemented";
        return false;
    }
    qCritical() << "not implemented";
    return false;
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

    if (m_events.contains(newEventType))
    {
        qWarning() << std::to_underlying(eventType) << "duplicate";
        return false;
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

bool ObjectEventsModel::changeEvent(const QString &object, const QString &newObject)
{
    const auto iter = m_collisionEvents.find(object);
    if (iter == std::cend(m_collisionEvents))
    {
        qWarning() << object << "not found";
        return false;
    }

    if (object == newObject)
    {
        qWarning() << "same event again";
        return true;
    }

    if (m_collisionEvents.contains(newObject))
    {
        qWarning() << newObject << "duplicate";
        return false;
    }

    auto container = std::move(iter->second);

    const auto oldRow = std::distance(std::begin(m_collisionEvents), iter);

    beginRemoveRows({}, oldRow + m_events.size(), oldRow + m_events.size());
    m_collisionEvents.erase(iter);
    endRemoveRows();

    // temporary copy to find row before inserting, as its needed for beginInsertRows()
    auto tempevents = m_collisionEvents;

    const auto &tempInsertResult = tempevents.insert(std::make_pair(newObject, ActionsContainer{}));
    if (!tempInsertResult.second)
    {
        qWarning() << "temp inserting failed!";
        return false;
    }

    const auto tempNewRow = std::distance(std::begin(tempevents), tempInsertResult.first);


    beginInsertRows({}, tempNewRow + m_events.size(), tempNewRow + m_events.size());

    const auto &insertResult = m_collisionEvents.insert(std::make_pair(newObject, std::move(container)));
    if (!insertResult.second)
    {
        qWarning() << "inserting failed!";
        return false;
    }

    const auto newRow = std::distance(std::begin(m_collisionEvents), insertResult.first);
    Q_ASSERT(tempNewRow == newRow);

    endInsertRows();

    return true;
}

bool ObjectEventsModel::removeEvent(std::variant<Object::EventType, QString> eventType)
{
    if (const auto &type = std::get_if<Object::EventType>(&eventType))
        return removeEvent(*type);
    if (const auto &object = std::get_if<QString>(&eventType))
        return removeEvent(*object);
    qCritical() << "not implemented";
    return false;
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

bool ObjectEventsModel::removeEvent(const QString &object)
{
    Q_UNUSED(object);
    qCritical() << "not implemented";
    return false;
}

auto ObjectEventsModel::getEvent(const QModelIndex &index) -> get_event_result_t
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return std::nullopt;
    }

    return getEvent(index.row());
}

auto ObjectEventsModel::getEvent(const QModelIndex &index) const -> const_get_event_result_t
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return std::nullopt;
    }

    return getEvent(index.row());
}

auto ObjectEventsModel::getEvent(int row) -> get_event_result_t
{
    if (row < 0)
    {
        qWarning() << "unexpected row" << row;
        return std::nullopt;
    }
    else if (std::size_t(row) < m_events.size())
    {
        auto iter = std::next(std::begin(m_events), row);
        return std::make_pair(iter->first, std::ref(iter->second));
    }
    else if (std::size_t(row) < m_events.size() + m_collisionEvents.size())
    {
        auto iter = std::next(std::begin(m_collisionEvents), row - m_events.size());
        return std::make_pair(iter->first, std::ref(iter->second));
    }
    else
    {
        qWarning() << "unexpected row" << row;
        return std::nullopt;
    }
}

auto ObjectEventsModel::getEvent(int row) const -> const_get_event_result_t
{
    if (row < 0)
    {
        qWarning() << "unexpected row" << row;
        return std::nullopt;
    }
    else if (std::size_t(row) >= m_events.size())
    {
        auto iter = std::next(std::cbegin(m_events), row);
        return std::make_pair(iter->first, std::cref(iter->second));
    }
    else if (std::size_t(row) >= m_events.size() + m_collisionEvents.size())
    {
        auto iter = std::next(std::cbegin(m_collisionEvents), row - m_events.size());
        return std::make_pair(iter->first, std::cref(iter->second));
    }
    else
    {
        qWarning() << "unexpected row" << row;
        return std::nullopt;
    }
}
