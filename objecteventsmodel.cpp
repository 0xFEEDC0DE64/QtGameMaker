#include "objecteventsmodel.h"

#include <QDebug>
#include <QIcon>

#include "futurecpp.h"

ObjectEventsModel::ObjectEventsModel(std::map<Object::EventType, QString> &events, QObject *parent) :
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
        case Object::EventType::Step:    return tr("Step");
        case Object::EventType::Destroy: return tr("Destroy");
        default:
            qWarning() << "unknown event type" << std::to_underlying(pair.first);
            return QString::number(std::to_underlying(pair.first));
        }
    case Qt::DecorationRole:
        switch (pair.first)
        {
        case Object::EventType::Create:  return QIcon{":/qtgameengine/icons/event-create.png"};
        case Object::EventType::Step:    return QIcon{":/qtgameengine/icons/event-step.png"};
        case Object::EventType::Destroy: return QIcon{":/qtgameengine/icons/event-destroy.png"};
        default:
            qWarning() << "unknown event type" << std::to_underlying(pair.first);
            return {};
        }
    }

    return {};
}
