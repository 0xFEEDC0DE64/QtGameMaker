#pragma once

#include <QAbstractListModel>

#include "projectcontainer.h"

class TimelineMomentsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimelineMomentsModel(TimeLine::moments_container_t &moments, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex{}) override;

    bool addMoment(TimeLine::moment_key_t moment);
    bool changeMoment(TimeLine::moment_key_t moment, TimeLine::moment_key_t newMoment);
    bool removeMoment(TimeLine::moment_key_t moment);
    bool clearMoments();

    std::pair<const TimeLine::moment_key_t, ActionsContainer> *getMoment(const QModelIndex &index);
    const std::pair<const TimeLine::moment_key_t, ActionsContainer> *getMoment(const QModelIndex &index) const;
    std::pair<const TimeLine::moment_key_t, ActionsContainer> *getMoment(int row);
    const std::pair<const TimeLine::moment_key_t, ActionsContainer> *getMoment(int row) const;

private:
    TimeLine::moments_container_t &m_moments;
};
