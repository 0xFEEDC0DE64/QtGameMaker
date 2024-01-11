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

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    ActionsContainer *actionsContainer() const { return m_actionsContainer; }
    void setActionsContainer(ActionsContainer *actionsContainer);

    Action *getAction(const QModelIndex &index);
    const Action *getAction(const QModelIndex &index) const;
    Action *getAction(int row);
    const Action *getAction(int row) const;

    void appendAction(Action &&action);
    void appendAction(const Action &action);

signals:
    void changed();

private:
    ActionsContainer *m_actionsContainer{};
};
