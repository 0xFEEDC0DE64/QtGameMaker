#include "projecttreemodel.h"

#include <QDebug>
#include <QPixmap>

#include <utility>

#include "futurecpp.h"
#include "projectcontainer.h"

enum {
    RowSprites,
    RowSounds,
    RowBackgrounds,
    RowPaths,
    RowScripts,
    RowFonts,
    RowTimeLines,
    RowObjects,
    RowRooms,
    RowGameInformation,
    RowGlobalGameSettings,
    RowExtensionPackages,
    NumberOfRows
};

ProjectTreeModel::ProjectTreeModel(ProjectContainer *project, QObject *parent) :
    QAbstractItemModel{parent},
    m_project{project}
{
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, std::to_underlying(NodeType::Root));
    else if (!parent.parent().isValid())
    {
        switch (nodeType(parent))
        {
        case NodeType::Root:
            switch (parent.row())
            {
            case RowSprites:     return createIndex(row, column, std::to_underlying(NodeType::Sprite));
            case RowSounds:      return createIndex(row, column, std::to_underlying(NodeType::Sound));
            case RowBackgrounds: return createIndex(row, column, std::to_underlying(NodeType::Background));
            case RowPaths:       return createIndex(row, column, std::to_underlying(NodeType::Path));
            case RowScripts:     return createIndex(row, column, std::to_underlying(NodeType::Script));
            case RowFonts:       return createIndex(row, column, std::to_underlying(NodeType::Font));
            default:
                qWarning() << "unexpected parent row" << parent.row();
                return {};
            }
        default:
            qWarning() << "unexpected parent type" << parent.internalId();
            return {};
        }
    }
    else
    {
        qWarning() << "unexpected parent";
        return {};
    }
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &child) const
{
    switch (nodeType(child))
    {
    case NodeType::Root:       return {};
    case NodeType::Sprite:     return spritesRoot();
    case NodeType::Sound:      return soundsRoot();
    case NodeType::Background: return backgroundsRoot();
    case NodeType::Path:       return pathsRoot();
    case NodeType::Script:     return scriptsRoot();
    case NodeType::Font:       return fontsRoot();
    default:
        qWarning() << "unexpected child type" << child.internalId();
        return {};
    }
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_project ? NumberOfRows : 0;

    switch (nodeType(parent))
    {
    case NodeType::Root:
        switch (parent.row())
        {
        case RowSprites:     return m_project ? m_project->sprites.size() : 0;
        case RowSounds:      return m_project ? m_project->sounds.size() : 0;
        case RowBackgrounds: return m_project ? m_project->backgrounds.size() : 0;
        case RowPaths:       return m_project ? m_project->paths.size() : 0;
        case RowScripts:     return m_project ? m_project->scripts.size() : 0;
        case RowFonts:       return m_project ? m_project->fonts.size() : 0;
        case RowTimeLines:
        case RowObjects:
        case RowRooms:
        case RowGameInformation:
        case RowGlobalGameSettings:
        case RowExtensionPackages:
            return 0;
        default:
            qWarning() << "unexpected parent row" << parent.row();
            return -1;
        }

        return {};
    case NodeType::Sprite:     return 0;
    case NodeType::Sound:      return 0;
    case NodeType::Background: return 0;
    case NodeType::Path:       return 0;
    case NodeType::Script:     return 0;
    case NodeType::Font:       return 0;
    default:
        qWarning() << "unexpected parent child type" << parent.internalId();
        return -1;
    }
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    switch (nodeType(parent))
    {
    case NodeType::Root:
        switch (parent.row())
        {
        case RowSprites:
        case RowSounds:
        case RowBackgrounds:
        case RowPaths:
        case RowScripts:
        case RowFonts:
        case RowTimeLines:
        case RowObjects:
        case RowRooms:
        case RowGameInformation:
        case RowGlobalGameSettings:
        case RowExtensionPackages:
            return 1;
        default:
            qWarning() << "unexpected parent row" << parent.row();
            return -1;
        }

        return {};
    case NodeType::Sprite:     return 1;
    case NodeType::Sound:      return 1;
    case NodeType::Background: return 1;
    case NodeType::Path:       return 1;
    case NodeType::Script:     return 1;
    case NodeType::Font:       return 1;
    default:
        qWarning() << "unexpected parent child type" << parent.internalId();
        return -1;
    }
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        qWarning() << "invalid index";
        return {};
    }

    switch (nodeType(index))
    {
    case NodeType::Root:
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (index.row())
            {
            case RowSprites:            return tr("Sprites");
            case RowSounds:             return tr("Sounds");
            case RowBackgrounds:        return tr("Backgrounds");
            case RowPaths:              return tr("Paths");
            case RowScripts:            return tr("Scripts");
            case RowFonts:              return tr("Fonts");
            case RowTimeLines:          return tr("Time Lines");
            case RowObjects:            return tr("Objects");
            case RowRooms:              return tr("Rooms");
            case RowGameInformation:    return tr("Game Information");
            case RowGlobalGameSettings: return tr("Global Game Settings");
            case RowExtensionPackages:  return tr("Extension Packages");
            default:
                qWarning() << "unexpected root row" << index.row();
                return {};
            }
        case Qt::DecorationRole:
            switch (index.row())
            {
            case RowSprites:
            case RowSounds:
            case RowBackgrounds:
            case RowPaths:
            case RowScripts:
            case RowFonts:
            case RowTimeLines:
            case RowObjects:
            case RowRooms:              return QPixmap{":/qtgameengine/icons/tree/folder.png"}.scaled(16, 16);
            case RowGameInformation:    return QPixmap{":/qtgameengine/icons/tree/game-information.png"}.scaled(16, 16);
            case RowGlobalGameSettings: return QPixmap{":/qtgameengine/icons/tree/global-game-settings.png"}.scaled(16, 16);
            case RowExtensionPackages:  return QPixmap{":/qtgameengine/icons/tree/extension-packages.png"}.scaled(16, 16);
            default:
                qWarning() << "unexpected root row" << index.row();
                return {};
            }

        default:
            return {};
        }
    case NodeType::Sprite:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->sprites.size())
        {
            qWarning() << "sprite index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->sprites), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            if (iter->pixmaps.empty())
            {
                qWarning() << "sprite has no pixmaps";
                return {};
            }
            return iter->pixmaps.front().scaled(16, 16, Qt::KeepAspectRatio);

        default:
            return {};
        }
    }
    case NodeType::Sound:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->sounds.size())
        {
            qWarning() << "sound index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->sounds), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            switch (iter->type)
            {
            case Sound::Type::Sound:
                return QPixmap{":/qtgameengine/icons/tree/sound-file.png"}.scaled(16, 16);
            case Sound::Type::Music:
                return QPixmap{":/qtgameengine/icons/tree/music-file.png"}.scaled(16, 16);
            default:
                qWarning() << "unexpected sound type" << std::to_underlying(iter->type);
                return {};
            }

        default:
            return {};
        }
    }
    case NodeType::Background:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->backgrounds.size())
        {
            qWarning() << "sprite index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->backgrounds), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            return iter->pixmap.scaled(16, 16, Qt::KeepAspectRatio);
        default:
            return {};
        }
    }
    case NodeType::Path:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->paths.size())
        {
            qWarning() << "path index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->paths), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            return QPixmap{":/qtgameengine/icons/tree/path-file.png"}.scaled(16, 16);
        default:
            return {};
        }
    }
    case NodeType::Script:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->scripts.size())
        {
            qWarning() << "script index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->scripts), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            return QPixmap{":/qtgameengine/icons/tree/script-file.png"}.scaled(16, 16);
        default:
            return {};
        }
    }
    case NodeType::Font:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() >= m_project->fonts.size())
        {
            qWarning() << "font index out of bounds" << index.row();
            return {};
        }

        auto iter = std::next(std::cbegin(m_project->fonts), index.row());

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return iter->name;
        case Qt::DecorationRole:
            return QPixmap{":/qtgameengine/icons/tree/font-file.png"}.scaled(16, 16);
        default:
            return {};
        }
    }
    default:
        qWarning() << "unexpected index type" << index.internalId();
        return -1;
    }
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractItemModel::flags(index);

    switch (nodeType(index))
    {
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    }

    return flags;
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (const auto nodeType = this->nodeType(index))
    {
    case NodeType::Sprite:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->sprites.size())
        {
            qWarning() << "unexpected sprite row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->sprites), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    case NodeType::Sound:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->sounds.size())
        {
            qWarning() << "unexpected sound row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->sounds), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    case NodeType::Background:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->backgrounds.size())
        {
            qWarning() << "unexpected background row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->backgrounds), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    case NodeType::Path:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->paths.size())
        {
            qWarning() << "unexpected path row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->paths), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    case NodeType::Script:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->scripts.size())
        {
            qWarning() << "unexpected script row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->scripts), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    case NodeType::Font:
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (index.row() < 0 || index.row() >= m_project->fonts.size())
        {
            qWarning() << "unexpected font row" << index.row();
            return false;
        }
        auto iter = std::next(std::begin(m_project->fonts), index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            iter->name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        default:
            qWarning() << "unexpected role" << role;
            return false;
        }
    }
    default:
        qWarning() << "unexpected nodeType" << std::to_underlying(nodeType);
        return false;
    }
}

bool ProjectTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    qDebug() << sourceParent << sourceRow << count;

    return false;
}

bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent == spritesRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->sprites.size())
        {
            qWarning() << "unexpected sprite row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("sprite%0").arg(i);
            if (std::none_of(std::cbegin(m_project->sprites), std::cend(m_project->sprites),
                             [&name](const Sprite &sprite){ return sprite.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->sprites.emplace(std::next(std::begin(m_project->sprites), row), Sprite { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else if (parent == soundsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->sounds.size())
        {
            qWarning() << "unexpected sound row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("sound%0").arg(i);
            if (std::none_of(std::cbegin(m_project->sounds), std::cend(m_project->sounds),
                             [&name](const Sound &sound){ return sound.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->sounds.emplace(std::next(std::begin(m_project->sounds), row), Sound { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else if (parent == backgroundsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->backgrounds.size())
        {
            qWarning() << "unexpected background row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("background%0").arg(i);
            if (std::none_of(std::cbegin(m_project->backgrounds), std::cend(m_project->backgrounds),
                             [&name](const Background &background){ return background.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->backgrounds.emplace(std::next(std::begin(m_project->backgrounds), row), Background { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else if (parent == pathsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->paths.size())
        {
            qWarning() << "unexpected path row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("path%0").arg(i);
            if (std::none_of(std::cbegin(m_project->paths), std::cend(m_project->paths),
                             [&name](const Path &path){ return path.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->paths.emplace(std::next(std::begin(m_project->paths), row), Path { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else if (parent == scriptsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->scripts.size())
        {
            qWarning() << "unexpected script row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("script%0").arg(i);
            if (std::none_of(std::cbegin(m_project->scripts), std::cend(m_project->scripts),
                             [&name](const Script &script){ return script.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->scripts.emplace(std::next(std::begin(m_project->scripts), row), Script { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else if (parent == fontsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row > m_project->scripts.size())
        {
            qWarning() << "unexpected script row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = QStringLiteral("font%0").arg(i);
            if (std::none_of(std::cbegin(m_project->fonts), std::cend(m_project->fonts),
                             [&name](const Font &font){ return font.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        m_project->fonts.emplace(std::next(std::begin(m_project->fonts), row), Font { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }
    else
    {
        qWarning() << "unexpected parent" << parent;
        return false;
    }
}

bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent == spritesRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->sprites.size())
        {
            qWarning() << "unexpected sprite row" << row;
            return false;
        }

        if (count < 0 || count > m_project->sprites.size() - row)
        {
            qWarning() << "unexpected sprite row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->sprites), row);
        auto end = std::next(begin, count);
        m_project->sprites.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else if (parent == soundsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->sounds.size())
        {
            qWarning() << "unexpected sound row" << row;
            return false;
        }

        if (count < 0 || count > m_project->sounds.size() - row)
        {
            qWarning() << "unexpected sound row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->sounds), row);
        auto end = std::next(begin, count);
        m_project->sounds.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else if (parent == backgroundsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->backgrounds.size())
        {
            qWarning() << "unexpected background row" << row;
            return false;
        }

        if (count < 0 || count > m_project->backgrounds.size() - row)
        {
            qWarning() << "unexpected background row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->backgrounds), row);
        auto end = std::next(begin, count);
        m_project->backgrounds.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else if (parent == pathsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->paths.size())
        {
            qWarning() << "unexpected path row" << row;
            return false;
        }

        if (count < 0 || count > m_project->paths.size() - row)
        {
            qWarning() << "unexpected path row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->paths), row);
        auto end = std::next(begin, count);
        m_project->paths.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else if (parent == scriptsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->scripts.size())
        {
            qWarning() << "unexpected script row" << row;
            return false;
        }

        if (count < 0 || count > m_project->scripts.size() - row)
        {
            qWarning() << "unexpected script row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->scripts), row);
        auto end = std::next(begin, count);
        m_project->scripts.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else if (parent == fontsRoot())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        if (row < 0 || row >= m_project->fonts.size())
        {
            qWarning() << "unexpected font row" << row;
            return false;
        }

        if (count < 0 || count > m_project->fonts.size() - row)
        {
            qWarning() << "unexpected font row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(m_project->fonts), row);
        auto end = std::next(begin, count);
        m_project->fonts.erase(begin, end);
        emit endRemoveRows();

        return true;
    }
    else
    {
        qWarning() << "unexpected parent" << parent;
        return false;
    }
}

Qt::DropActions ProjectTreeModel::supportedDragActions() const
{
    return QAbstractItemModel::supportedDragActions() | Qt::MoveAction; // TOOD
}

Qt::DropActions ProjectTreeModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction; // TODO
}

void ProjectTreeModel::setProject(ProjectContainer *project)
{
    beginResetModel();
    m_project = project;
    endResetModel();
}

ProjectTreeModel::NodeType ProjectTreeModel::nodeType(const QModelIndex &index) const
{
    if (!index.isValid())
        qWarning() << "invalid index";

    return NodeType(index.internalId());
}

QModelIndex ProjectTreeModel::spritesRoot() const
{
    return createIndex(RowSprites, 0,     std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::soundsRoot() const
{
    return createIndex(RowSounds, 0,      std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::backgroundsRoot() const
{
    return createIndex(RowBackgrounds, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::pathsRoot() const
{
    return createIndex(RowPaths, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::scriptsRoot() const
{
    return createIndex(RowScripts, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::fontsRoot() const
{
    return createIndex(RowFonts, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::timeLinesRoot() const
{
    return createIndex(RowTimeLines, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::objectsRoot() const
{
    return createIndex(RowObjects, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::roomsRoot() const
{
    return createIndex(RowRooms, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::gameInformationRoot() const
{
    return createIndex(RowGameInformation, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::globalGameSettingsRoot() const
{
    return createIndex(RowGlobalGameSettings, 0, std::to_underlying(NodeType::Root));
}

QModelIndex ProjectTreeModel::extensionPackagesRoot() const
{
    return createIndex(RowExtensionPackages, 0, std::to_underlying(NodeType::Root));
}

Sprite *ProjectTreeModel::getSprite(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Sprite)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->sprites.size())
    {
        qWarning() << "sprite index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->sprites), index.row());
}

const Sprite *ProjectTreeModel::getSprite(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Sprite)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->sprites.size())
    {
        qWarning() << "sprite index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->sprites), index.row());
}

Sound *ProjectTreeModel::getSound(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Sound)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->sounds.size())
    {
        qWarning() << "sound index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->sounds), index.row());
}

const Sound *ProjectTreeModel::getSound(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Sound)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->sounds.size())
    {
        qWarning() << "sound index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->sounds), index.row());
}

Background *ProjectTreeModel::getBackground(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Background)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->backgrounds.size())
    {
        qWarning() << "background index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->backgrounds), index.row());
}

const Background *ProjectTreeModel::getBackground(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Background)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->backgrounds.size())
    {
        qWarning() << "background index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->backgrounds), index.row());
}

Path *ProjectTreeModel::getPath(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Path)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->paths.size())
    {
        qWarning() << "path index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->paths), index.row());
}

const Path *ProjectTreeModel::getPath(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Path)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->paths.size())
    {
        qWarning() << "path index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->paths), index.row());
}

Script *ProjectTreeModel::getScript(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Script)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->scripts.size())
    {
        qWarning() << "script index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->scripts), index.row());
}

const Script *ProjectTreeModel::getScript(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Script)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->scripts.size())
    {
        qWarning() << "script index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->scripts), index.row());
}

Font *ProjectTreeModel::getFont(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Font)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->fonts.size())
    {
        qWarning() << "font index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::begin(m_project->fonts), index.row());
}

const Font *ProjectTreeModel::getFont(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != NodeType::Font)
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    if (index.row() < 0)
    {
        qWarning() << "unexpected negative row" << index.row();
        return nullptr;
    }

    if (index.row() >= m_project->fonts.size())
    {
        qWarning() << "font index out of bounds" << index.row();
        return {};
    }

    return &*std::next(std::cbegin(m_project->fonts), index.row());
}
