#pragma once

#include <QAbstractListModel>

#include <optional>

#include "projectcontainer.h"

class ActionsContainerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ActionsContainerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    ActionsContainer *actionsContainer() const { return m_actionsContainer; }
    void setActionsContainer(ActionsContainer *actionsContainer);

    Action *getAction(const QModelIndex &index);
    const Action *getAction(const QModelIndex &index) const;
    Action *getAction(int row);
    const Action *getAction(int row) const;

private:
    ActionsContainer *m_actionsContainer{};
};
