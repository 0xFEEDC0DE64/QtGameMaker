#include "actionscontainermodel.h"

#include <QDebug>
#include <QIcon>
#include <QMimeData>

#include "projectserialization.h"
#include "stdserialization.h"

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
    // actions |= Qt::TargetMoveAction;
    return actions;
}

Qt::DropActions ActionsContainerModel::supportedDragActions() const
{
    auto actions = QAbstractListModel::supportedDragActions();
    actions |= Qt::MoveAction;
    // actions |= Qt::TargetMoveAction;
    return actions;
}

QStringList ActionsContainerModel::mimeTypes() const
{
    return {"custom"};
}

QMimeData *ActionsContainerModel::mimeData(const QModelIndexList &indexes) const
{
    if (!m_actionsContainer)
        return nullptr;
    if (indexes.size() <= 0)
        return nullptr;

    QStringList types = mimeTypes();
    if (types.isEmpty())
        return nullptr;
    QMimeData *data = new QMimeData;
    QString format = types.at(0);
    QByteArray encoded;
    QDataStream stream(&encoded, QDataStream::WriteOnly);
    for (const auto &index : indexes)
    {
        const Action &action = *std::next(std::cbegin(*m_actionsContainer), index.row());
        stream << action;
    }
    data->setData(format, encoded);
    return data;
}

bool ActionsContainerModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (!m_actionsContainer)
    {
        qWarning() << "no container";
        return false;
    }
    // if the drop is on an item, reject drop
    if (parent.isValid() && row == -1 && column == -1)
    {
        // qWarning() << "drop on item";
        return false;
    }

    return QAbstractListModel::canDropMimeData(data, action, row, column, parent);
}

bool ActionsContainerModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!m_actionsContainer)
    {
        qWarning() << "no container";
        return false;
    }
    // check if the action is supported
    if (!data || !(action == Qt::CopyAction || action == Qt::MoveAction))
    {
        qWarning() << "wrong action";
        return false;
    }

    // if the drop is on an item, replace the data in the items
    if (parent.isValid() && row == -1 && column == -1)
    {
        // qWarning() << "drop on item";
        return false;
    }

    // check if the format is supported
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return false;
    QString format = types.at(0);
    if (!data->hasFormat(format))
    {
        qWarning() << "wrong format";
        return false;
    }
    if (row > rowCount(parent))
        row = rowCount(parent);
    if (row == -1)
        row = rowCount(parent);
    if (column == -1)
        column = 0;
    // decode and insert
    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QDataStream::ReadOnly);

    QList<Action> actions;
    while (!stream.atEnd())
    {
        Action action;
        stream >> action;
        actions << std::move(action);
    }
    if (actions.isEmpty())
    {
        qWarning() << "empty actions";
        return false;
    }

    beginInsertRows({}, row, row + actions.size() - 1);
    auto iter = std::next(std::begin(*m_actionsContainer), row);
    for (auto &action : actions)
        iter = m_actionsContainer->insert(iter, std::move(action));
    endInsertRows();

    if (!actions.isEmpty())
        emit changed();

    return true;
}

bool ActionsContainerModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << row << count << parent.isValid();

    if (!m_actionsContainer)
        return false;
    if (parent.isValid())
        return false;

    if (row < 0 || std::size_t(row) >= m_actionsContainer->size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_actionsContainer->size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);
    auto begin = std::next(std::begin(*m_actionsContainer), row);
    auto end = std::next(begin, count);
    m_actionsContainer->erase(begin, end);
    endRemoveRows();

    if (count)
        emit changed();

    return true;
}

bool ActionsContainerModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    qDebug() << sourceRow << count << destinationChild;

    if (!m_actionsContainer)
        return false;
    if (sourceParent.isValid())
        return false;
    if (destinationParent.isValid())
        return false;

    if (count != 1)
        return false;

    beginMoveRows({}, sourceRow, sourceRow, {}, destinationChild);
    m_actionsContainer->splice(std::next(std::begin(*m_actionsContainer), destinationChild), *m_actionsContainer, std::next(std::begin(*m_actionsContainer), sourceRow));
    endMoveRows();

    emit changed();

    return true;
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

void ActionsContainerModel::appendAction(Action &&action)
{
    if (!m_actionsContainer)
    {
        qWarning() << "invalid actions container";
        return;
    }

    beginInsertRows({}, m_actionsContainer->size(), m_actionsContainer->size());
    m_actionsContainer->push_back(std::move(action));
    endInsertRows();
    emit changed();
}

void ActionsContainerModel::appendAction(const Action &action)
{
    if (!m_actionsContainer)
    {
        qWarning() << "invalid actions container";
        return;
    }

    beginInsertRows({}, m_actionsContainer->size(), m_actionsContainer->size());
    m_actionsContainer->push_back(action);
    endInsertRows();
    emit changed();
}
