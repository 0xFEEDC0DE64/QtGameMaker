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

    const auto &action = *std::next(m_actionsContainer->cbegin(), index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (std::holds_alternative<MoveFixedAction>(action))
            return tr("Start moving in a direction");
        else if (std::holds_alternative<MoveFreeAction>(action))
            return tr("Set direction and speed of motion");
        else if (std::holds_alternative<MoveTowardsAction>(action))
            return tr("Move towards point (99, 99)");
        else if (std::holds_alternative<ExecuteCodeAction>(action))
            return tr("Execute a piece of code");
        else
        {
            qWarning() << "unknown action type";
            return tr("ERROR: Unknown action type");
        }
    case Qt::DecorationRole:
        if (std::holds_alternative<MoveFixedAction>(action))
            return QIcon{":/qtgameengine/icons/action-move-fixed.png"};
        else if (std::holds_alternative<MoveFreeAction>(action))
            return QIcon{":/qtgameengine/icons/action-move-free.png"};
        else if (std::holds_alternative<MoveTowardsAction>(action))
            return QIcon{":/qtgameengine/icons/action-move-towards.png"};
        else if (std::holds_alternative<ExecuteCodeAction>(action))
            return QIcon{":/qtgameengine/icons/action-code.png"};
        else
        {
            qWarning() << "unknown action type";
            return QIcon{":/qtgameengine/icons/action.png"};
        }
    default:
        return {};
    }

    return {};
}

Qt::ItemFlags ActionsContainerModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractListModel::flags(index);
    flags |= Qt::ItemIsDragEnabled;
    flags |= Qt::ItemIsDropEnabled;
    return flags;
}

Qt::DropActions ActionsContainerModel::supportedDropActions() const
{
    auto actions = QAbstractListModel::supportedDropActions();
    actions |= Qt::MoveAction;
    actions |= Qt::TargetMoveAction;
    return actions;
}

Qt::DropActions ActionsContainerModel::supportedDragActions() const
{
    auto actions = QAbstractListModel::supportedDragActions();
    actions |= Qt::MoveAction;
    actions |= Qt::TargetMoveAction;
    return actions;
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
