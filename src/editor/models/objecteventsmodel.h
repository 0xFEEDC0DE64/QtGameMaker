#pragma once

#include <QAbstractListModel>

#include <map>

#include "projectcontainer.h"

class ObjectEventsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ObjectEventsModel(Object::events_container_t &events, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex{}) override;

    bool addEvent(Object::EventType eventType);
    bool changeEvent(Object::EventType eventType, Object::EventType newEventType);
    bool removeEvent(Object::EventType eventType);

    std::pair<const Object::EventType, ActionsContainer> *getEvent(const QModelIndex &index);
    const std::pair<const Object::EventType, ActionsContainer> *getEvent(const QModelIndex &index) const;
    std::pair<const Object::EventType, ActionsContainer> *getEvent(int row);
    const std::pair<const Object::EventType, ActionsContainer> *getEvent(int row) const;

private:
    Object::events_container_t &m_events;
};
