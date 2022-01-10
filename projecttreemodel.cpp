#include "projecttreemodel.h"

#include <QDebug>
#include <QPixmap>

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
            case RowRooms:              return QPixmap{":/qtgameengine/icons/folder.png"}.scaled(16, 16);
            case RowGameInformation:    return QPixmap{":/qtgameengine/icons/game-information-file.png"}.scaled(16, 16);
            case RowGlobalGameSettings: return QPixmap{":/qtgameengine/icons/global-game-settings-file.png"}.scaled(16, 16);
            case RowExtensionPackages:  return QPixmap{":/qtgameengine/icons/extension-packages-file.png"}.scaled(16, 16);
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
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
    case NodeType::TimeLine:
    case NodeType::Object:
    case NodeType::Room:
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    }

    return flags;
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

bool ProjectTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    qDebug() << sourceParent << sourceRow << count;

    return false;
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
    if (index.row() < 0 || index.row() >= container.size())
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
    if (index.row() < 0 || index.row() >= container.size())
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
bool ProjectTreeModel::rename(const T &entry, const QString &newName)
{
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

    iter->name = newName;

    emitNameChanged<T>(*iter);
    const auto index = this->index(std::distance(std::begin(container), iter), 0, rootFor<T>());
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

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

template<typename T>
QVariant ProjectTreeModel::dataFor(const QModelIndex &index, int role) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    const auto &container = m_project->containerFor<T>();
    if (index.row() >= container.size())
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
    return entry.pixmaps.front().scaled(16, 16, Qt::IgnoreAspectRatio /*KeepAspectRatio*/);
}

template<>
QVariant ProjectTreeModel::iconFor<Sound>(const Sound &entry) const
{
    switch (entry.type)
    {
    case Sound::Type::Sound:
        return QPixmap{":/qtgameengine/icons/sound-file.png"}.scaled(16, 16);
    case Sound::Type::Music:
        return QPixmap{":/qtgameengine/icons/music-file.png"}.scaled(16, 16);
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
        QPixmap pixmap{16, 16};
        pixmap.fill(Qt::white);
        return pixmap;
    }
    return entry.pixmap.scaled(16, 16, Qt::KeepAspectRatio);
}

template<>
QVariant ProjectTreeModel::iconFor<Path>(const Path &entry) const
{
    return QPixmap{":/qtgameengine/icons/path-file.png"}.scaled(16, 16);
}

template<>
QVariant ProjectTreeModel::iconFor<Script>(const Script &entry) const
{
    return QPixmap{":/qtgameengine/icons/script-file.png"}.scaled(16, 16);
}

template<>
QVariant ProjectTreeModel::iconFor<Font>(const Font &entry) const
{
    return QPixmap{":/qtgameengine/icons/font-file.png"}.scaled(16, 16);
}

template<>
QVariant ProjectTreeModel::iconFor<TimeLine>(const TimeLine &entry) const
{
    return QPixmap{":/qtgameengine/icons/timeline-file.png"}.scaled(16, 16);
}

template<>
QVariant ProjectTreeModel::iconFor<Object>(const Object &entry) const
{
    return QPixmap{":/qtgameengine/icons/object-file.png"}.scaled(16, 16);
}

template<>
QVariant ProjectTreeModel::iconFor<Room>(const Room &entry) const
{
    return QPixmap{":/qtgameengine/icons/room-file.png"}.scaled(16, 16);
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
    if (index.row() < 0 || index.row() >= container.size())
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
            iter->name = std::move(name);
            emitNameChanged<T>(*iter);
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
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
    if (parent == rootFor<T>())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return {};
        }

        auto &container = m_project->containerFor<T>();
        if (row < 0 || row > container.size())
        {
            qWarning() << "unexpected row" << row;
            return false;
        }

        QString name;
        for (int i = 0; ; i++)
        {
            name = nameTempateFor<T>().arg(i);
            if (std::none_of(std::cbegin(container), std::cend(container),
                             [&name](const T &entry){ return entry.name == name; }))
                break;
        }

        emit beginInsertRows(parent, row, row + count - 1);
        container.emplace(std::next(std::begin(container), row), T { .name = std::move(name) });
        emit endInsertRows();

        return true;
    }

    return std::nullopt;
}

template<typename T>
std::optional<bool> ProjectTreeModel::removeRowsFor(int row, int count, const QModelIndex &parent)
{
    if (parent == rootFor<T>())
    {
        if (!m_project)
        {
            qWarning() << "unexpected null project";
            return false;
        }

        auto &container = m_project->containerFor<T>();

        if (row < 0 || row >= container.size())
        {
            qWarning() << "unexpected row" << row;
            return false;
        }

        if (count < 0 || count > container.size() - row)
        {
            qWarning() << "unexpected row+count" << count << row;
            return false;
        }

        emit beginRemoveRows(parent, row, row + count - 1);
        auto begin = std::next(std::begin(container), row);
        auto end = std::next(begin, count);
        container.erase(begin, end);
        emit endRemoveRows();

        return true;
    }

    return std::nullopt;
}

template<>
void ProjectTreeModel::emitNameChanged<Sprite>(const Sprite &entry)
{
    emit spriteNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Sound>(const Sound &entry)
{
    emit soundNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Background>(const Background &entry)
{
    emit backgroundNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Path>(const Path &entry)
{
    emit pathNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Script>(const Script &entry)
{
    emit scriptNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Font>(const Font &entry)
{
    emit fontNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<TimeLine>(const TimeLine &entry)
{
    emit timeLineNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Object>(const Object &entry)
{
    emit objectNameChanged(entry);
}

template<>
void ProjectTreeModel::emitNameChanged<Room>(const Room &entry)
{
    emit roomNameChanged(entry);
}

template<>
QString ProjectTreeModel::nameTempateFor<Sprite>()
{
    return QStringLiteral("sprite%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Sound>()
{
    return QStringLiteral("sound%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Background>()
{
    return QStringLiteral("background%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Path>()
{
    return QStringLiteral("path%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Script>()
{
    return QStringLiteral("script%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Font>()
{
    return QStringLiteral("font%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<TimeLine>()
{
    return QStringLiteral("timeline%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Object>()
{
    return QStringLiteral("object%0");
}

template<>
QString ProjectTreeModel::nameTempateFor<Room>()
{
    return QStringLiteral("room%0");
}
