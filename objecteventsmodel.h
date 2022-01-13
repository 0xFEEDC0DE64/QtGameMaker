#pragma once

#include <QAbstractListModel>

#include <map>

#include "projectcontainer.h"

class ObjectEventsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ObjectEventsModel(std::map<Object::EventType, QString> &events, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    using QAbstractListModel::beginResetModel;
    using QAbstractListModel::endResetModel;

private:
    std::map<Object::EventType, QString> &m_events;
};
