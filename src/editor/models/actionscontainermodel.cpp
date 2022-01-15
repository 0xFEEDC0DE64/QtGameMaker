#include "actionscontainermodel.h"

#include <QDebug>
#include <QIcon>

ActionsContainerModel::ActionsContainerModel(QObject *parent) :
    QAbstractListModel{parent}
{
}

int ActionsContainerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_actionsContainer ? m_actionsContainer->size() : 0;
}

QVariant ActionsContainerModel::data(const QModelIndex &index, int role) const
{
    if (!m_actionsContainer)
    {
        qWarning() << "no currentEvent";
        return {};
    }

    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return {};
    }

    if (index.row() < 0 || std::size_t(index.row()) >= m_actionsContainer->size())
    {
        qWarning() << "unexpected row" << index.row();
        return {};
    }

    const auto &action = m_actionsContainer->at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return tr("Execute a piece of code");
    case Qt::DecorationRole:
        return QIcon{":/qtgameengine/icons/code-action.png"};
    default:
        return {};
    }

    return {};
}

void ActionsContainerModel::setActionsContainer(ActionsContainer *actionsContainer)
{
    beginResetModel();
    m_actionsContainer = actionsContainer;
    endResetModel();
}

Action *ActionsContainerModel::getAction(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getAction(index.row());
}

const Action *ActionsContainerModel::getAction(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return nullptr;
    }

    return getAction(index.row());
}

Action *ActionsContainerModel::getAction(int row)
{
    if (!m_actionsContainer)
    {
        qWarning() << "invalid actions container";
        return nullptr;
    }

    if (row < 0 || std::size_t(row) >= m_actionsContainer->size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::begin(*m_actionsContainer), row);
}

const Action *ActionsContainerModel::getAction(int row) const
{
    if (!m_actionsContainer)
    {
        qWarning() << "invalid actions container";
        return nullptr;
    }

    if (row < 0 || std::size_t(row) >= m_actionsContainer->size())
    {
        qWarning() << "unexpected row" << row;
        return nullptr;
    }

    return &*std::next(std::cbegin(*m_actionsContainer), row);
}
