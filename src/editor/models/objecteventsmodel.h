#pragma once

#include <QAbstractListModel>

#include <map>

#include "projectcontainer.h"

class ObjectEventsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ObjectEventsModel(Object::events_container_t &events, Object::collision_events_container_t &collisionEvents,
                               QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex{}) override;

    bool addEvent(std::variant<Object::EventType, QString> eventType);
    bool addEvent(Object::EventType eventType);
    bool addEvent(const QString &object);
    bool changeEvent(std::variant<Object::EventType, QString> eventType, std::variant<Object::EventType, QString> newEventType);
    bool changeEvent(Object::EventType eventType, Object::EventType newEventType);
    bool changeEvent(const QString &object, const QString &newObject);
    bool removeEvent(std::variant<Object::EventType, QString> eventType);
    bool removeEvent(Object::EventType eventType);
    bool removeEvent(const QString &object);

    using get_event_result_t = std::optional<std::pair<std::variant<Object::EventType, QString>, std::reference_wrapper<ActionsContainer>>>;
    using const_get_event_result_t = std::optional<std::pair<std::variant<Object::EventType, QString>, std::reference_wrapper<const ActionsContainer>>>;

    get_event_result_t getEvent(const QModelIndex &index);
    const_get_event_result_t getEvent(const QModelIndex &index) const;
    get_event_result_t getEvent(int row);
    const_get_event_result_t getEvent(int row) const;

private:
    Object::events_container_t &m_events;
    Object::collision_events_container_t &m_collisionEvents;
};
