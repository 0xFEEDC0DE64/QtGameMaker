#include "projecttreemodel.h"

#include <QDebug>
#include <QPixmap>
#include <QIcon>

#include <utility>
#include <algorithm>

#include "futurecpp.h"
#include "projectcontainer.h"

namespace {
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
}

ProjectTreeModel::ProjectTreeModel(QObject *parent) :
    QAbstractItemModel{parent}
{
}

ProjectTreeModel::ProjectTreeModel(ProjectContainer &project, QObject *parent) :
    QAbstractItemModel{parent},
    m_project{&project}
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
            case RowTimeLines:   return createIndex(row, column, std::to_underlying(NodeType::TimeLine));
            case RowObjects:     return createIndex(row, column, std::to_underlying(NodeType::Object));
            case RowRooms:       return createIndex(row, column, std::to_underlying(NodeType::Room));
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
    case NodeType::Sprite:     return rootFor<Sprite>();
    case NodeType::Sound:      return rootFor<Sound>();
    case NodeType::Background: return rootFor<Background>();
    case NodeType::Path:       return rootFor<Path>();
    case NodeType::Script:     return rootFor<Script>();
    case NodeType::Font:       return rootFor<Font>();
    case NodeType::TimeLine:   return rootFor<TimeLine>();
    case NodeType::Object:     return rootFor<Object>();
    case NodeType::Room:       return rootFor<Room>();
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
        case RowSprites:     return m_project ? m_project->containerFor<Sprite>().size() : 0;
        case RowSounds:      return m_project ? m_project->containerFor<Sound>().size() : 0;
        case RowBackgrounds: return m_project ? m_project->containerFor<Background>().size() : 0;
        case RowPaths:       return m_project ? m_project->containerFor<Path>().size() : 0;
        case RowScripts:     return m_project ? m_project->containerFor<Script>().size() : 0;
        case RowFonts:       return m_project ? m_project->containerFor<Font>().size() : 0;
        case RowTimeLines:   return m_project ? m_project->containerFor<TimeLine>().size() : 0;
        case RowObjects:     return m_project ? m_project->containerFor<Object>().size() : 0;
        case RowRooms:       return m_project ? m_project->containerFor<Room>().size() : 0;
        case RowGameInformation:
        case RowGlobalGameSettings:
        case RowExtensionPackages:
            return 0;
        default:
            qWarning() << "unexpected parent row" << parent.row();
            return -1;
        }

        return {};
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
    case NodeType::TimeLine:
    case NodeType::Object:
    case NodeType::Room:
        return 0;
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
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
    case NodeType::TimeLine:
    case NodeType::Object:
    case NodeType::Room:
        return 1;
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
            case RowRooms:              return QIcon{":/qtgameengine/icons/folder.png"};
            case RowGameInformation:    return QIcon{":/qtgameengine/icons/game-information-file.png"};
            case RowGlobalGameSettings: return QIcon{":/qtgameengine/icons/global-game-settings-file.png"};
            case RowExtensionPackages:  return QIcon{":/qtgameengine/icons/extension-packages-file.png"};
            default:
                qWarning() << "unexpected root row" << index.row();
                return {};
            }

        default:
            return {};
        }
    case NodeType::Sprite:     return dataFor<Sprite>(index, role);
    case NodeType::Sound:      return dataFor<Sound>(index, role);
    case NodeType::Background: return dataFor<Background>(index, role);
    case NodeType::Path:       return dataFor<Path>(index, role);
    case NodeType::Script:     return dataFor<Script>(index, role);
    case NodeType::Font:       return dataFor<Font>(index, role);
    case NodeType::TimeLine:   return dataFor<TimeLine>(index, role);
    case NodeType::Object:     return dataFor<Object>(index, role);
    case NodeType::Room:       return dataFor<Room>(index, role);
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
    case NodeType::Root:
        break;
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
    case NodeType::TimeLine:
    case NodeType::Object:
    case NodeType::Room:
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }

    return flags;
}

Qt::DropActions ProjectTreeModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (const auto nodeType = this->nodeType(index))
    {
    case NodeType::Sprite:     return setDataFor<Sprite>(index, value, role);
    case NodeType::Sound:      return setDataFor<Sound>(index, value, role);
    case NodeType::Background: return setDataFor<Background>(index, value, role);
    case NodeType::Path:       return setDataFor<Path>(index, value, role);
    case NodeType::Script:     return setDataFor<Script>(index, value, role);
    case NodeType::Font:       return setDataFor<Font>(index, value, role);
    case NodeType::TimeLine:   return setDataFor<TimeLine>(index, value, role);
    case NodeType::Object:     return setDataFor<Object>(index, value, role);
    case NodeType::Room:       return setDataFor<Room>(index, value, role);
    default:
        qWarning() << "unexpected nodeType" << std::to_underlying(nodeType);
        return false;
    }
}

bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (auto result = insertRowsFor<Sprite>(row, count, parent))     return *result;
    if (auto result = insertRowsFor<Sound>(row, count, parent))      return *result;
    if (auto result = insertRowsFor<Background>(row, count, parent)) return *result;
    if (auto result = insertRowsFor<Path>(row, count, parent))       return *result;
    if (auto result = insertRowsFor<Script>(row, count, parent))     return *result;
    if (auto result = insertRowsFor<Font>(row, count, parent))       return *result;
    if (auto result = insertRowsFor<TimeLine>(row, count, parent))   return *result;
    if (auto result = insertRowsFor<Object>(row, count, parent))     return *result;
    if (auto result = insertRowsFor<Room>(row, count, parent))       return *result;

    qWarning() << "unexpected parent" << parent;
    return false;
}

bool ProjectTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    qDebug() << "called" << sourceParent << sourceRow << count << destinationParent << destinationChild;
    return false;
}

bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (auto result = removeRowsFor<Sprite>(row, count, parent))     return *result;
    if (auto result = removeRowsFor<Sound>(row, count, parent))      return *result;
    if (auto result = removeRowsFor<Background>(row, count, parent)) return *result;
    if (auto result = removeRowsFor<Path>(row, count, parent))       return *result;
    if (auto result = removeRowsFor<Script>(row, count, parent))     return *result;
    if (auto result = removeRowsFor<Font>(row, count, parent))       return *result;
    if (auto result = removeRowsFor<TimeLine>(row, count, parent))   return *result;
    if (auto result = removeRowsFor<Object>(row, count, parent))     return *result;
    if (auto result = removeRowsFor<Room>(row, count, parent))       return *result;

    qWarning() << "unexpected parent" << parent;
    return false;
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

template<> QModelIndex ProjectTreeModel::rootFor<Sprite>() const
{
    return createIndex(RowSprites, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Sound>() const
{
    return createIndex(RowSounds, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Background>() const
{
    return createIndex(RowBackgrounds, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Path>() const
{
    return createIndex(RowPaths, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Script>() const
{
    return createIndex(RowScripts, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Font>() const
{
    return createIndex(RowFonts, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<TimeLine>() const
{
    return createIndex(RowTimeLines, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Object>() const
{
    return createIndex(RowObjects, 0, std::to_underlying(NodeType::Root));
}

template<> QModelIndex ProjectTreeModel::rootFor<Room>() const
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

template<typename T>
T *ProjectTreeModel::get(const QModelIndex &index)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != nodeTypeFor<T>())
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    auto &container = m_project->containerFor<T>();
    if (index.row() < 0 || std::size_t(index.row()) >= container.size())
    {
        qWarning() << "invalid row" << index.row();
        return nullptr;
    }

    return &*std::next(std::begin(container), index.row());
}

template<typename T>
const T *ProjectTreeModel::get(const QModelIndex &index) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    if (const auto nodeType = this->nodeType(index); nodeType != nodeTypeFor<T>())
    {
        qWarning() << "unexpected index type" << std::to_underlying(nodeType);
        return nullptr;
    }

    const auto &container = m_project->containerFor<T>();
    if (index.row() < 0 || std::size_t(index.row()) >= container.size())
    {
        qWarning() << "invalid row" << index.row();
        return nullptr;
    }

    return &*std::next(std::cbegin(container), index.row());
}

template Sprite *ProjectTreeModel::get<Sprite>(const QModelIndex &index);
template const Sprite *ProjectTreeModel::get<Sprite>(const QModelIndex &index) const;
template Sound *ProjectTreeModel::get<Sound>(const QModelIndex &index);
template const Sound *ProjectTreeModel::get<Sound>(const QModelIndex &index) const;
template Background *ProjectTreeModel::get<Background>(const QModelIndex &index);
template const Background *ProjectTreeModel::get<Background>(const QModelIndex &index) const;
template Path *ProjectTreeModel::get<Path>(const QModelIndex &index);
template const Path *ProjectTreeModel::get<Path>(const QModelIndex &index) const;
template Script *ProjectTreeModel::get<Script>(const QModelIndex &index);
template const Script *ProjectTreeModel::get<Script>(const QModelIndex &index) const;
template Font *ProjectTreeModel::get<Font>(const QModelIndex &index);
template const Font *ProjectTreeModel::get<Font>(const QModelIndex &index) const;
template TimeLine *ProjectTreeModel::get<TimeLine>(const QModelIndex &index);
template const TimeLine *ProjectTreeModel::get<TimeLine>(const QModelIndex &index) const;
template Object *ProjectTreeModel::get<Object>(const QModelIndex &index);
template const Object *ProjectTreeModel::get<Object>(const QModelIndex &index) const;
template Room *ProjectTreeModel::get<Room>(const QModelIndex &index);
template const Room *ProjectTreeModel::get<Room>(const QModelIndex &index) const;

template<typename T>
T *ProjectTreeModel::create()
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return nullptr;
    }

    auto &container = m_project->containerFor<T>();

    beginInsertRows(rootFor<T>(), container.size(), container.size());
    container.emplace_back(T { .name = getFreeNameFor<T>(container) });
    endInsertRows();

    return &container.back();
}

template Sprite *ProjectTreeModel::create<Sprite>();
template Sound *ProjectTreeModel::create<Sound>();
template Background *ProjectTreeModel::create<Background>();
template Path *ProjectTreeModel::create<Path>();
template Script *ProjectTreeModel::create<Script>();
template Font *ProjectTreeModel::create<Font>();
template TimeLine *ProjectTreeModel::create<TimeLine>();
template Object *ProjectTreeModel::create<Object>();
template Room *ProjectTreeModel::create<Room>();

template<typename T>
bool ProjectTreeModel::remove(const T &entry)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();
    const auto iter = std::find_if(std::begin(container), std::end(container),
                                   [&entry](const auto &otherEntry){ return &entry == &otherEntry; });
    if (iter == std::cend(container))
    {
        qWarning() << "entry not from this project!";
        return false;
    }

    const auto row = std::distance(std::begin(container), iter);

    onBeforeRemove<T>(*iter);

    emitAboutToBeRemoved<T>(*iter);

    beginRemoveRows(rootFor<T>(), row, row);
    container.erase(iter);
    endRemoveRows();

    return true;
}

template bool ProjectTreeModel::remove<Sprite>(const Sprite &entry);
template bool ProjectTreeModel::remove<Sound>(const Sound &entry);
template bool ProjectTreeModel::remove<Background>(const Background &entry);
template bool ProjectTreeModel::remove<Path>(const Path &entry);
template bool ProjectTreeModel::remove<Script>(const Script &entry);
template bool ProjectTreeModel::remove<Font>(const Font &entry);
template bool ProjectTreeModel::remove<TimeLine>(const TimeLine &entry);
template bool ProjectTreeModel::remove<Object>(const Object &entry);
template bool ProjectTreeModel::remove<Room>(const Room &entry);

template<typename T>
bool ProjectTreeModel::rename(const T &entry, const QString &newName)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();
    const auto iter = std::find_if(std::begin(container), std::end(container),
                                   [&entry](const auto &otherEntry){ return &entry == &otherEntry; });
    if (iter == std::cend(container))
    {
        qWarning() << "entry not from this project!";
        return false;
    }

    if (iter->name == newName)
        return true;

    if (std::any_of(std::cbegin(container), std::cend(container),
                    [&newName](const auto &entry){ return entry.name == newName; }))
    {
        qWarning() << "duplicate name" << newName;
        return false;
    }

    onBeforeRename<T>(*iter, newName);

    auto oldName = std::move(iter->name);
    iter->name = std::move(newName);

    emitNameChanged<T>(*iter, oldName);
    const auto index = this->index(std::distance(std::begin(container), iter), 0, rootFor<T>());
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    onAfterRename<T>(*iter, std::move(oldName));

    return true;
}

template bool ProjectTreeModel::rename<Sprite>(const Sprite &entry, const QString &newName);
template bool ProjectTreeModel::rename<Sound>(const Sound &entry, const QString &newName);
template bool ProjectTreeModel::rename<Background>(const Background &entry, const QString &newName);
template bool ProjectTreeModel::rename<Path>(const Path &entry, const QString &newName);
template bool ProjectTreeModel::rename<Script>(const Script &entry, const QString &newName);
template bool ProjectTreeModel::rename<Font>(const Font &entry, const QString &newName);
template bool ProjectTreeModel::rename<TimeLine>(const TimeLine &entry, const QString &newName);
template bool ProjectTreeModel::rename<Object>(const Object &entry, const QString &newName);
template bool ProjectTreeModel::rename<Room>(const Room &entry, const QString &newName);

bool ProjectTreeModel::setSpritePixmaps(const Sprite &sprite, std::vector<QPixmap> &&pixmaps)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    {
        auto &container = m_project->sprites;
        const auto iter = std::find_if(std::begin(container), std::end(container),
                                       [&sprite](const auto &otherEntry){ return &sprite == &otherEntry; });
        if (iter == std::cend(container))
        {
            qWarning() << "sprite not from this project!";
            return false;
        }

        iter->pixmaps = std::move(pixmaps);

        const auto index = this->index(std::distance(std::begin(container), iter), 0, rootFor<Sprite>());
        emit dataChanged(index, index, { Qt::DecorationRole });

        emit spritePixmapsChanged(*iter);
    }

    for (auto iter = std::cbegin(m_project->objects); iter != std::cend(m_project->objects); iter++)
    {
        if (iter->spriteName != sprite.name)
            continue;

        const auto index = this->index(std::distance(std::cbegin(m_project->objects), iter), 0, rootFor<Object>());
        emit dataChanged(index, index, {Qt::DecorationRole});
    }

    return true;
}

bool ProjectTreeModel::setBackgroundPixmap(const Background &background, QPixmap &&pixmap)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->backgrounds;
    const auto iter = std::find_if(std::begin(container), std::end(container),
                                   [&background](const auto &otherEntry){ return &background == &otherEntry; });
    if (iter == std::cend(container))
    {
        qWarning() << "background not from this project!";
        return false;
    }

    iter->pixmap = std::move(pixmap);

    const auto index = this->index(std::distance(std::begin(container), iter), 0, rootFor<Background>());
    emit dataChanged(index, index, { Qt::DecorationRole });

    emit backgroundPixmapChanged(*iter);

    return true;
}

bool ProjectTreeModel::setObjectSpriteName(const Object &object, QString &&spriteName)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->objects;
    const auto iter = std::find_if(std::begin(container), std::end(container),
                                   [&object](const auto &otherEntry){ return &object == &otherEntry; });
    if (iter == std::cend(container))
    {
        qWarning() << "object not from this project!";
        return false;
    }

    if (iter->spriteName == spriteName)
        return true;

    auto oldSpriteName = std::move(iter->spriteName);
    iter->spriteName = std::move(spriteName);

    const auto index = this->index(std::distance(std::begin(container), iter), 0, rootFor<Object>());
    emit dataChanged(index, index, { Qt::DecorationRole });

    emit objectSpriteNameChanged(object, std::move(oldSpriteName));

    return true;
}

template<typename T>
QVariant ProjectTreeModel::dataFor(const QModelIndex &index, int role) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    const auto &container = m_project->containerFor<T>();
    if (std::size_t(index.row()) >= container.size())
    {
        qWarning() << "index out of bounds" << index.row();
        return {};
    }

    auto iter = std::next(std::cbegin(container), index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return iter->name;
    case Qt::DecorationRole:
        return iconFor<T>(*iter);

    default:
        return {};
    }
}

template<>
QVariant ProjectTreeModel::iconFor<Sprite>(const Sprite &entry) const
{
    if (entry.pixmaps.empty() || entry.pixmaps.front().isNull())
    {
        QPixmap pixmap{16, 16};
        pixmap.fill(Qt::white);
        return pixmap;
    }
    return QIcon{entry.pixmaps.front()};
}

template<>
QVariant ProjectTreeModel::iconFor<Sound>(const Sound &entry) const
{
    switch (entry.type)
    {
    case Sound::Type::Sound:
        return QIcon{":/qtgameengine/icons/sound-file.png"};
    case Sound::Type::Music:
        return QIcon{":/qtgameengine/icons/music-file.png"};
    default:
        qWarning() << "unexpected sound type" << std::to_underlying(entry.type);
        return {};
    }
}

template<>
QVariant ProjectTreeModel::iconFor<Background>(const Background &entry) const
{
    if (entry.pixmap.isNull())
    {
        QPixmap pixmap{64, 64};
        pixmap.fill(Qt::white);
        return QIcon{pixmap};
    }
    return QIcon{entry.pixmap};
}

template<>
QVariant ProjectTreeModel::iconFor<Path>(const Path &entry) const
{
    Q_UNUSED(entry)
    return QIcon{":/qtgameengine/icons/path-file.png"};
}

template<>
QVariant ProjectTreeModel::iconFor<Script>(const Script &entry) const
{
    Q_UNUSED(entry)
    return QIcon{":/qtgameengine/icons/script-file.png"};
}

template<>
QVariant ProjectTreeModel::iconFor<Font>(const Font &entry) const
{
    Q_UNUSED(entry)
    return QIcon{":/qtgameengine/icons/font-file.png"};
}

template<>
QVariant ProjectTreeModel::iconFor<TimeLine>(const TimeLine &entry) const
{
    Q_UNUSED(entry)
    return QIcon{":/qtgameengine/icons/timeline-file.png"};
}

template<>
QVariant ProjectTreeModel::iconFor<Object>(const Object &entry) const
{
    if (m_project && !entry.spriteName.isEmpty())
    {
        const auto iter = std::find_if(std::cbegin(m_project->sprites), std::cend(m_project->sprites),
                                       [&](const Sprite &sprite){ return sprite.name == entry.spriteName; });
        if (iter == std::cend(m_project->sprites))
            qWarning() << "sprite" << entry.spriteName << "not found";
        else if (!iter->pixmaps.empty() && !iter->pixmaps.front().isNull())
            return QIcon{iter->pixmaps.front()};
    }

    QPixmap pixmap{64, 64};
    pixmap.fill(Qt::white);
    return QIcon{pixmap};
}

template<>
QVariant ProjectTreeModel::iconFor<Room>(const Room &entry) const
{
    Q_UNUSED(entry)
    return QIcon{":/qtgameengine/icons/room-file.png"};
}

template<typename T>
bool ProjectTreeModel::setDataFor(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    auto &container = m_project->containerFor<T>();
    if (index.row() < 0 || std::size_t(index.row()) >= container.size())
    {
        qWarning() << "unexpected row" << index.row();
        return false;
    }
    auto iter = std::next(std::begin(container), index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (auto name = value.toString(); name != iter->name)
        {
            if (std::any_of(std::cbegin(container), std::cend(container),
                            [&name](const auto &entry){ return entry.name == name; }))
            {
                qWarning() << "duplicate name" << name;
                emit errorOccured(tr("A Sprite with the name \"%0\" is already existing").arg(name));
                return false;
            }
            onBeforeRename<T>(*iter, name);
            auto oldName = std::move(iter->name);
            iter->name = std::move(name);
            emitNameChanged<T>(*iter, oldName);
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            onAfterRename<T>(*iter, std::move(oldName));
        }
        return true;
    default:
        qWarning() << "unexpected role" << role;
        return false;
    }
}

template<typename T>
std::optional<bool> ProjectTreeModel::insertRowsFor(int row, int count, const QModelIndex &parent)
{
    if (parent != rootFor<T>())
        return std::nullopt;

    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();
    if (row < 0 || std::size_t(row) > container.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    for (size_t i = 0; i < std::size_t(count); i++)
        container.emplace(std::next(std::begin(container), row + i), T { .name = getFreeNameFor<T>(container) });
    endInsertRows();

    return true;
}

template<typename T>
std::optional<bool> ProjectTreeModel::removeRowsFor(int row, int count, const QModelIndex &parent)
{
    if (parent != rootFor<T>())
        return std::nullopt;

    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();

    if (row < 0 || std::size_t(row) >= container.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > container.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    auto begin = std::next(std::begin(container), row);
    auto end = std::next(begin, count);

    for (auto iter = begin; iter != end; iter++)
        onBeforeRemove<T>(*iter);

    for (auto iter = begin; iter != end; iter++)
        emitAboutToBeRemoved(*iter);

    beginRemoveRows(parent, row, row + count - 1);
    container.erase(begin, end);
    endRemoveRows();

    return true;
}

template<typename T> void ProjectTreeModel::onBeforeRemove(const T &entry)
{
    Q_UNUSED(entry)
}

template<> void ProjectTreeModel::onBeforeRemove<Sprite>(const Sprite &sprite)
{
    for (auto iter = std::begin(m_project->objects); iter != std::end(m_project->objects); iter++)
    {
        if (iter->spriteName != sprite.name)
            continue;

        auto oldSpriteName = std::move(iter->spriteName);
        iter->spriteName.clear();

        const auto index = this->index(std::distance(std::begin(m_project->objects), iter), 0, rootFor<Object>());
        emit dataChanged(index, index, {Qt::DecorationRole});

        emit objectSpriteNameChanged(*iter, std::move(oldSpriteName));
    }
}

template<> void ProjectTreeModel::onBeforeRemove<Object>(const Object &object)
{
    for (auto &room : m_project->rooms)
        for (auto iter = std::begin(room.objects); iter != std::end(room.objects); )
            if (iter->objectName == object.name)
                iter = room.objects.erase(iter);
            else
                iter++;
}

template<typename T> void ProjectTreeModel::onBeforeRename(const T &entry, const QString &newName)
{
    Q_UNUSED(entry)
    Q_UNUSED(newName)
}

template<typename T> void ProjectTreeModel::onAfterRename(const T &entry, const QString &oldName)
{
    Q_UNUSED(entry)
    Q_UNUSED(oldName)
}

template<> void ProjectTreeModel::onAfterRename<Sprite>(const Sprite &sprite, const QString &oldName)
{
    for (auto &object : m_project->objects)
    {
        if (object.spriteName != oldName)
            continue;

        auto oldSpriteName = std::move(object.spriteName);

        object.spriteName = sprite.name;

//        const auto index = this->index(std::distance(std::begin(m_project->objects), iter), 0, rootFor<Object>());
//        emit dataChanged(index, index, {Qt::DecorationRole});

        emit objectSpriteNameChanged(object, std::move(oldSpriteName));
    }
}

template<> void ProjectTreeModel::onAfterRename<Object>(const Object &object, const QString &oldName)
{
    for (auto &room : m_project->rooms)
    {
        for (auto &obj : room.objects)
        {
            if (obj.objectName != oldName)
                continue;

            obj.objectName = object.name;
        }
    }

    // TODO object collision events
}

template<typename T>
QString ProjectTreeModel::getFreeNameFor(const std::list<T> &container)
{
    QString name;
    for (int i = 0; ; i++)
    {
        name = nameTempateFor<T>().arg(i);
        if (std::none_of(std::cbegin(container), std::cend(container),
                         [&name](const T &entry){ return entry.name == name; }))
            break;
    }
    return name;
}

template<> void ProjectTreeModel::emitCreated<Sprite>(const Sprite &entry) { emit spriteCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Sound>(const Sound &entry) { emit soundCreated(entry);  }
template<> void ProjectTreeModel::emitCreated<Background>(const Background &entry) { emit backgroundCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Path>(const Path &entry) { emit pathCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Script>(const Script &entry) { emit scriptCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Font>(const Font &entry) { emit fontCreated(entry); }
template<> void ProjectTreeModel::emitCreated<TimeLine>(const TimeLine &entry) { emit timeLineCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Object>(const Object &entry) { emit objectCreated(entry); }
template<> void ProjectTreeModel::emitCreated<Room>(const Room &entry) { emit roomCreated(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Sprite>(const Sprite &entry) { emit spriteAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Sound>(const Sound &entry) { emit soundAboutToBeRemoved(entry);  }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Background>(const Background &entry) { emit backgroundAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Path>(const Path &entry) { emit pathAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Script>(const Script &entry) { emit scriptAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Font>(const Font &entry) { emit fontAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<TimeLine>(const TimeLine &entry) { emit timeLineAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Object>(const Object &entry) { emit objectAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitAboutToBeRemoved<Room>(const Room &entry) { emit roomAboutToBeRemoved(entry); }
template<> void ProjectTreeModel::emitNameChanged<Sprite>(const Sprite &entry, const QString &oldName) { emit spriteNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Sound>(const Sound &entry, const QString &oldName) { emit soundNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Background>(const Background &entry, const QString &oldName) { emit backgroundNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Path>(const Path &entry, const QString &oldName) { emit pathNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Script>(const Script &entry, const QString &oldName) { emit scriptNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Font>(const Font &entry, const QString &oldName) { emit fontNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<TimeLine>(const TimeLine &entry, const QString &oldName) { emit timeLineNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Object>(const Object &entry, const QString &oldName) { emit objectNameChanged(entry, oldName); }
template<> void ProjectTreeModel::emitNameChanged<Room>(const Room &entry, const QString &oldName) { emit roomNameChanged(entry, oldName); }
template<> QString ProjectTreeModel::nameTempateFor<Sprite>() { return QStringLiteral("sprite%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Sound>() { return QStringLiteral("sound%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Background>() { return QStringLiteral("background%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Path>() { return QStringLiteral("path%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Script>() { return QStringLiteral("script%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Font>() { return QStringLiteral("font%0"); }
template<> QString ProjectTreeModel::nameTempateFor<TimeLine>() { return QStringLiteral("timeline%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Object>() { return QStringLiteral("object%0"); }
template<> QString ProjectTreeModel::nameTempateFor<Room>() { return QStringLiteral("room%0"); }
