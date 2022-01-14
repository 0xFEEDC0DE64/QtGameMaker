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

    if (index.row() < 0 || index.row() >= m_events.size())
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
    if (row < 0 || row >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || count > m_events.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    emit beginRemoveRows(parent, row, row + count - 1);
    auto begin = std::next(std::begin(m_events), row);
    auto end = std::next(begin, count);
    m_events.erase(begin, end);
    emit endRemoveRows();

    return true;
}

bool ObjectEventsModel::addEvent(Object::EventType eventType)
{
    if (m_events.contains(eventType))
    {
        qWarning() << std::to_underlying(eventType) << "duplicate";
        return false;
    }

    beginResetModel();
    m_events[eventType];
    endResetModel();

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

    beginResetModel();
    auto value = std::move(iter->second);
    m_events.erase(iter);
    m_events[newEventType] = std::move(value);
    endResetModel();

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

    beginResetModel();
    m_events.erase(iter);
    endResetModel();

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
    if (row < 0 || row >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::begin(m_events), row);
}

const std::pair<const Object::EventType, ActionsContainer> *ObjectEventsModel::getEvent(int row) const
{
    if (row < 0 || row >= m_events.size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::cbegin(m_events), row);
}
