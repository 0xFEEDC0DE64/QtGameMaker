#include "projecttreemodel.h"

#include <QDebug>
#include <QPixmap>
#include <QIcon>
#include <QMimeData>
#include <QPersistentModelIndex>

#include <utility>
#include <algorithm>

#include "futurecpp.h"
#include "projectcontainer.h"

namespace {
constexpr auto ResourceMimeType = "application/x-qtgamemaker-resource-node";

class ResourceMimeData : public QMimeData
{
public:
    explicit ResourceMimeData(const QModelIndex &index) : source{index} {
        setData(ResourceMimeType, QByteArray{1, 'x'});
    }
    QPersistentModelIndex source;
};

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

template<typename Callback>
void visitActions(ActionsContainer &actions, Callback &&callback)
{
    for (Action &action : actions)
        std::visit([&](auto &value) { callback(value.appliesTo); }, action);
}

template<typename Callback>
void visitProjectActions(ProjectContainer &project, Callback &&callback)
{
    for (Object &object : project.objects)
    {
        for (auto &[event, actions] : object.events)
            visitActions(actions, callback);
        for (auto &[event, actions] : object.collisionEvents)
            visitActions(actions, callback);
    }
    for (TimeLine &timeline : project.timeLines)
        for (auto &[moment, actions] : timeline.moments)
            visitActions(actions, callback);
}

template<typename T>
bool findNode(ResourceFolder<T> &folder, const void *pointer,
              ResourceFolder<T> *&owner, ResourceNode<T> *&node)
{
    for (const auto &child : folder.children.template get<0>())
    {
        if (child.get() == pointer)
        {
            owner = &folder;
            node = child.get();
            return true;
        }
        if (auto *nested = child->folder(); nested && findNode(*nested, pointer, owner, node))
            return true;
    }
    return false;
}

template<typename T>
bool findFolderNode(ResourceFolder<T> &folder, const ResourceFolder<T> *target,
                    ResourceFolder<T> *&owner, ResourceNode<T> *&node)
{
    for (const auto &child : folder.children.template get<0>())
    {
        if (child->folder() == target)
        {
            owner = &folder;
            node = child.get();
            return true;
        }
        if (auto *nested = child->folder(); nested && findFolderNode(*nested, target, owner, node))
            return true;
    }
    return false;
}
}

template<typename T>
std::pair<ResourceFolder<T> *, ResourceNode<T> *>
ProjectTreeModel::locationFor(const QModelIndex &index) const
{
    if (!m_project || !index.isValid() || index.internalId() == std::to_underlying(NodeType::Root))
        return {nullptr, nullptr};
    ResourceFolder<T> *owner{};
    ResourceNode<T> *node{};
    findNode(const_cast<ResourceFolder<T> &>(m_project->containerFor<T>().root),
             index.internalPointer(), owner, node);
    return {owner, node};
}

template<typename T>
ResourceFolder<T> *ProjectTreeModel::folderFor(const QModelIndex &index) const
{
    if (!m_project) return nullptr;
    if (index == rootFor<T>())
        return &const_cast<ResourceFolder<T> &>(m_project->containerFor<T>().root);
    auto [owner, node] = locationFor<T>(index);
    return node ? node->folder() : nullptr;
}

template<typename T>
QModelIndex ProjectTreeModel::indexForResource(const T &resource) const
{
    if (!m_project) return {};
    auto [owner, node] = const_cast<ResourceCollection<T> &>(m_project->containerFor<T>()).findNode(&resource);
    return owner && node ? createIndex(owner->rowOf(node), 0, node) : QModelIndex{};
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

template<typename T>
QVariant ProjectTreeModel::dataFor(const QModelIndex &index, int role) const
{
    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return {};
    }

    auto [owner, node] = locationFor<T>(index);
    if (!owner || !node || !node->resource()) return {};
    const T &resource = *node->resource();

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return resource.name;
    case Qt::DecorationRole:
        return iconFor<T>(resource);

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

    auto [owner, node] = locationFor<T>(index);
    if (!owner || !node || !node->resource()) return false;
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return rename<T>(*node->resource(), value.toString());
    default:
        qWarning() << "unexpected role" << role;
        return false;
    }
}

template<typename T>
std::optional<bool> ProjectTreeModel::insertRowsFor(int row, int count, const QModelIndex &parent)
{
    auto *folder = folderFor<T>(parent);
    if (!folder)
        return std::nullopt;

    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();
    if (row < 0 || std::size_t(row) > folder->children.size() || count <= 0)
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    auto &sequence = folder->children.template get<0>();
    for (int i = 0; i < count; i++)
    {
        sequence.insert(std::next(sequence.begin(), row + i),
                        std::make_unique<ResourceNode<T>>(T{.name = getFreeNameFor<T>(*folder)}));
        container.invalidate();
    }
    endInsertRows();

    return true;
}

template<typename T>
std::optional<bool> ProjectTreeModel::removeRowsFor(int row, int count, const QModelIndex &parent)
{
    auto *folder = folderFor<T>(parent);
    if (!folder)
        return std::nullopt;

    if (!m_project)
    {
        qWarning() << "unexpected null project";
        return false;
    }

    auto &container = m_project->containerFor<T>();

    if (row < 0 || std::size_t(row) >= folder->children.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count <= 0 || std::size_t(count) > folder->children.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    auto &sequence = folder->children.template get<0>();
    auto begin = std::next(sequence.begin(), row);
    auto end = std::next(begin, count);

    const auto notifyDescendants = [this](const auto &self, ResourceNode<T> &node) -> void {
        if (T *resource = node.resource())
        {
            onBeforeRemove<T>(*resource);
            emitAboutToBeRemoved(*resource);
        }
        else if (auto *nested = node.folder())
            for (const auto &child : nested->children.template get<0>())
                self(self, *child);
    };
    for (auto iter = begin; iter != end; ++iter)
        notifyDescendants(notifyDescendants, **iter);

    beginRemoveRows(parent, row, row + count - 1);
    sequence.erase(begin, end);
    container.invalidate();
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
        if (iter->spriteName == sprite.name)
        {
            auto oldSpriteName = std::move(iter->spriteName);
            iter->spriteName.clear();

            const auto index = indexForResource(*iter);
            emit dataChanged(index, index, {Qt::DecorationRole});

            emit objectSpriteNameChanged(*iter, std::move(oldSpriteName));
        }

        if (iter->maskSpriteName == sprite.name)
        {
            iter->maskSpriteName.clear();
        }
    }
}

template<> void ProjectTreeModel::onBeforeRemove<Object>(const Object &object)
{
    visitProjectActions(*m_project, [&object](AppliesTo &appliesTo) {
        if (auto *target = std::get_if<AppliesToObject>(&appliesTo);
            target && target->objectName == object.name)
            target->objectName.clear();
    });

    for (Object &obj : m_project->objects)
    {
        if (!obj.parentName.isEmpty() && obj.parentName == object.name)
            obj.parentName.clear();

        obj.collisionEvents.erase(object.name);
    }

    for (Room &room : m_project->rooms)
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
        if (object.spriteName == oldName)
        {
            auto oldSpriteName = std::move(object.spriteName);

            object.spriteName = sprite.name;

            const auto index = indexForResource(object);
            emit dataChanged(index, index, {Qt::DecorationRole});

            emit objectSpriteNameChanged(object, std::move(oldSpriteName));
        }

        if (object.maskSpriteName == oldName)
        {
            object.maskSpriteName = sprite.name;
        }
    }
}

template<> void ProjectTreeModel::onBeforeRename<Object>(const Object &object, const QString &newName)
{
    visitProjectActions(*m_project, [&object, &newName](AppliesTo &appliesTo) {
        if (auto *target = std::get_if<AppliesToObject>(&appliesTo);
            target && target->objectName == object.name)
            target->objectName = newName;
    });

    for (Object &obj : m_project->objects)
    {
        if (!obj.parentName.isEmpty() && obj.parentName == object.name)
            obj.parentName = newName;

        if (const auto iter = obj.collisionEvents.find(object.name); iter != std::end(obj.collisionEvents))
        {
            auto node = obj.collisionEvents.extract(iter);
            node.key() = newName;
            obj.collisionEvents.insert(std::move(node));
        }
    }

    for (auto &room : m_project->rooms)
    {
        for (auto &obj : room.objects)
        {
            if (obj.objectName != object.name)
                continue;

            obj.objectName = newName;
        }
    }
}

template<typename T>
QString ProjectTreeModel::getFreeNameFor(const ResourceFolder<T> &folder) const
{
    QString name;
    for (int i = 0; ; i++)
    {
        name = nameTempateFor<T>().arg(i);
        if (!m_project->hasResourceName(name) &&
            folder.children.template get<1>().find(name) == folder.children.template get<1>().end())
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

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0 || !m_project) return {};
    if (!parent.isValid())
        return row < NumberOfRows ? createIndex(row, column, std::to_underlying(NodeType::Root)) : QModelIndex{};
#define RESOURCE_INDEX(T) if (auto *folder = folderFor<T>(parent)) { \
        auto *node = folder->at(row); return node ? createIndex(row, column, node) : QModelIndex{}; }
    RESOURCE_INDEX(Sprite)
    RESOURCE_INDEX(Sound)
    RESOURCE_INDEX(Background)
    RESOURCE_INDEX(Path)
    RESOURCE_INDEX(Script)
    RESOURCE_INDEX(Font)
    RESOURCE_INDEX(TimeLine)
    RESOURCE_INDEX(Object)
    RESOURCE_INDEX(Room)
#undef RESOURCE_INDEX
    return {};
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalId() == std::to_underlying(NodeType::Root)) return {};
#define RESOURCE_PARENT(T) { \
        auto [owner, node] = locationFor<T>(child); \
        if (owner && node) { \
            auto &root = const_cast<ResourceFolder<T> &>(m_project->containerFor<T>().root); \
            if (owner == &root) return rootFor<T>(); \
            ResourceFolder<T> *grandOwner{}; ResourceNode<T> *folderNode{}; \
            findFolderNode(root, owner, grandOwner, folderNode); \
            return grandOwner && folderNode ? createIndex(grandOwner->rowOf(folderNode), 0, folderNode) : QModelIndex{}; \
        } }
    RESOURCE_PARENT(Sprite)
    RESOURCE_PARENT(Sound)
    RESOURCE_PARENT(Background)
    RESOURCE_PARENT(Path)
    RESOURCE_PARENT(Script)
    RESOURCE_PARENT(Font)
    RESOURCE_PARENT(TimeLine)
    RESOURCE_PARENT(Object)
    RESOURCE_PARENT(Room)
#undef RESOURCE_PARENT
    return {};
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
        case RowSprites:     return m_project ? m_project->containerFor<Sprite>().root.children.size() : 0;
        case RowSounds:      return m_project ? m_project->containerFor<Sound>().root.children.size() : 0;
        case RowBackgrounds: return m_project ? m_project->containerFor<Background>().root.children.size() : 0;
        case RowPaths:       return m_project ? m_project->containerFor<Path>().root.children.size() : 0;
        case RowScripts:     return m_project ? m_project->containerFor<Script>().root.children.size() : 0;
        case RowFonts:       return m_project ? m_project->containerFor<Font>().root.children.size() : 0;
        case RowTimeLines:   return m_project ? m_project->containerFor<TimeLine>().root.children.size() : 0;
        case RowObjects:     return m_project ? m_project->containerFor<Object>().root.children.size() : 0;
        case RowRooms:       return m_project ? m_project->containerFor<Room>().root.children.size() : 0;
        case RowGameInformation:
        case RowGlobalGameSettings:
        case RowExtensionPackages:
            return 0;
        default:
            qWarning() << "unexpected parent row" << parent.row();
            return -1;
        }

        return {};
    case NodeType::Folder:
#define FOLDER_ROWS(T) if (auto *folder = folderFor<T>(parent)) return int(folder->children.size());
        FOLDER_ROWS(Sprite)
        FOLDER_ROWS(Sound)
        FOLDER_ROWS(Background)
        FOLDER_ROWS(Path)
        FOLDER_ROWS(Script)
        FOLDER_ROWS(Font)
        FOLDER_ROWS(TimeLine)
        FOLDER_ROWS(Object)
        FOLDER_ROWS(Room)
#undef FOLDER_ROWS
        return 0;
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
    case NodeType::Folder:
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
    case NodeType::Folder:
    {
#define FOLDER_DATA(T) { auto [owner, node] = locationFor<T>(index); \
        if (owner && node && node->folder()) { \
            if (role == Qt::DisplayRole || role == Qt::EditRole) return node->name(); \
            if (role == Qt::DecorationRole) return QIcon{":/qtgameengine/icons/folder.png"}; \
            return {}; \
        } }
        FOLDER_DATA(Sprite)
        FOLDER_DATA(Sound)
        FOLDER_DATA(Background)
        FOLDER_DATA(Path)
        FOLDER_DATA(Script)
        FOLDER_DATA(Font)
        FOLDER_DATA(TimeLine)
        FOLDER_DATA(Object)
        FOLDER_DATA(Room)
#undef FOLDER_DATA
        return {};
    }
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
    if (!index.isValid()) return Qt::NoItemFlags;
    auto flags = QAbstractItemModel::flags(index);

    switch (nodeType(index))
    {
    case NodeType::Root:
        if (index.row() <= RowRooms)
            flags |= Qt::ItemIsDropEnabled;
        break;
    case NodeType::Folder:
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    }

    return flags;
}

Qt::DropActions ProjectTreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList ProjectTreeModel::mimeTypes() const
{
    return {QString::fromLatin1(ResourceMimeType)};
}

QMimeData *ProjectTreeModel::mimeData(const QModelIndexList &indexes) const
{
    for (const QModelIndex &index : indexes)
        if (index.isValid() && nodeType(index) != NodeType::Root)
            return new ResourceMimeData(index);
    return nullptr;
}

bool ProjectTreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action,
                                       int row, int column, const QModelIndex &parent) const
{
    if (action != Qt::MoveAction || column > 0 || !data) return false;
    const auto *resourceData = dynamic_cast<const ResourceMimeData *>(data);
    if (!resourceData || !resourceData->source.isValid() || resourceData->source.model() != this) return false;
    if (nodeType(parent) != NodeType::Root && nodeType(parent) != NodeType::Folder) return false;
    const QModelIndex source = resourceData->source;
    const auto categoryRoot = [](QModelIndex index) {
        while (index.isValid() && index.parent().isValid())
            index = index.parent();
        return index;
    };
    if (!parent.isValid() || categoryRoot(source) != categoryRoot(parent)) return false;
    if (row < -1 || row > rowCount(parent)) return false;
    for (QModelIndex ancestor = parent; ancestor.isValid(); ancestor = ancestor.parent())
        if (ancestor == source) return false;
    const int destinationRow = row < 0 ? rowCount(parent) : row;
    if (source.parent() == parent &&
        (destinationRow == source.row() || destinationRow == source.row() + 1)) return false;
    if (source.parent() != parent)
    {
        const QString sourceName = source.data(Qt::DisplayRole).toString();
        for (int child = 0; child < rowCount(parent); ++child)
            if (this->data(index(child, 0, parent), Qt::DisplayRole).toString() == sourceName)
                return false;
    }
    return true;
}

bool ProjectTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                    int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent)) return false;
    const auto *resourceData = static_cast<const ResourceMimeData *>(data);
    const QModelIndex source = resourceData->source;
    return moveRows(source.parent(), source.row(), 1, parent,
                    row < 0 ? rowCount(parent) : row);
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (const auto nodeType = this->nodeType(index))
    {
    case NodeType::Folder:
    {
        if (role != Qt::EditRole && role != Qt::DisplayRole) return false;
        const QString name = value.toString().trimmed();
        if (name.isEmpty()) return false;
#define RENAME_FOLDER(T) { auto [owner, node] = locationFor<T>(index); \
            if (owner && node && node->folder()) { \
                if (node->name() == name) return true; \
                auto &hashed = owner->children.template get<1>(); \
                if (hashed.find(name) != hashed.end()) return false; \
                auto &sequence = owner->children.template get<0>(); \
                auto it = std::find_if(sequence.begin(), sequence.end(), \
                    [node](const auto &child) { return child.get() == node; }); \
                const bool okay = sequence.modify(it, [&name](auto &child) { child->folder()->name = name; }); \
                if (okay) emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole}); \
                return okay; \
            } }
        RENAME_FOLDER(Sprite)
        RENAME_FOLDER(Sound)
        RENAME_FOLDER(Background)
        RENAME_FOLDER(Path)
        RENAME_FOLDER(Script)
        RENAME_FOLDER(Font)
        RENAME_FOLDER(TimeLine)
        RENAME_FOLDER(Object)
        RENAME_FOLDER(Room)
#undef RENAME_FOLDER
        return false;
    }
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
    if (count <= 0 || sourceRow < 0 || destinationChild < 0) return false;
#define MOVE_RESOURCES(T) { \
        auto *source = folderFor<T>(sourceParent); \
        auto *destination = folderFor<T>(destinationParent); \
        if (source && destination) { \
            auto &from = source->children.template get<0>(); \
            auto &to = destination->children.template get<0>(); \
            if (std::size_t(sourceRow + count) > from.size() || std::size_t(destinationChild) > to.size()) return false; \
            if (source == destination && destinationChild >= sourceRow && destinationChild <= sourceRow + count) return false; \
            for (int i = 0; i < count; ++i) { \
                auto *moving = source->at(sourceRow + i); \
                if (source != destination && destination->children.template get<1>().find(moving->name()) != destination->children.template get<1>().end()) return false; \
                if (auto *nested = moving->folder()) { \
                    const auto contains = [&](const auto &self, const ResourceFolder<T> *folder) -> bool { \
                        if (folder == destination) return true; \
                        for (const auto &child : folder->children.template get<0>()) \
                            if (const auto *childFolder = child->folder(); childFolder && self(self, childFolder)) return true; \
                        return false; \
                    }; \
                    if (contains(contains, nested)) return false; \
                } \
            } \
            if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild)) return false; \
            auto first = std::next(from.begin(), sourceRow); \
            auto last = std::next(first, count); \
            to.splice(std::next(to.begin(), destinationChild), from, first, last); \
            m_project->containerFor<T>().invalidate(); \
            endMoveRows(); \
            return true; \
        } }
    MOVE_RESOURCES(Sprite)
    MOVE_RESOURCES(Sound)
    MOVE_RESOURCES(Background)
    MOVE_RESOURCES(Path)
    MOVE_RESOURCES(Script)
    MOVE_RESOURCES(Font)
    MOVE_RESOURCES(TimeLine)
    MOVE_RESOURCES(Object)
    MOVE_RESOURCES(Room)
#undef MOVE_RESOURCES
    return false;
}

QModelIndex ProjectTreeModel::createFolder(const QModelIndex &parent, const QString &requestedName, int requestedRow)
{
    QString name = requestedName.trimmed();
#define CREATE_FOLDER(T) if (auto *folder = folderFor<T>(parent)) { \
        auto &sequence = folder->children.template get<0>(); \
        if (name.isEmpty()) { \
            for (int i = 0;; ++i) { \
                name = tr("group%1").arg(i); \
                if (folder->children.template get<1>().find(name) == folder->children.template get<1>().end()) break; \
            } \
        } \
        if (folder->children.template get<1>().find(name) != folder->children.template get<1>().end()) return {}; \
        const int row = requestedRow < 0 ? int(sequence.size()) : requestedRow; \
        if (row < 0 || std::size_t(row) > sequence.size()) return {}; \
        beginInsertRows(parent, row, row); \
        auto node = std::make_unique<ResourceNode<T>>(std::make_unique<ResourceFolder<T>>(name)); \
        auto *pointer = node.get(); \
        sequence.insert(std::next(sequence.begin(), row), std::move(node)); \
        endInsertRows(); \
        return createIndex(row, 0, pointer); \
    }
    CREATE_FOLDER(Sprite)
    CREATE_FOLDER(Sound)
    CREATE_FOLDER(Background)
    CREATE_FOLDER(Path)
    CREATE_FOLDER(Script)
    CREATE_FOLDER(Font)
    CREATE_FOLDER(TimeLine)
    CREATE_FOLDER(Object)
    CREATE_FOLDER(Room)
#undef CREATE_FOLDER
    return {};
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
    if (!index.isValid()) return NodeType::Root;
    if (index.internalId() == std::to_underlying(NodeType::Root)) return NodeType::Root;
#define RESOURCE_TYPE(T) { auto [owner, node] = locationFor<T>(index); \
        if (owner && node) return node->isFolder() ? NodeType::Folder : nodeTypeFor<T>(); }
    RESOURCE_TYPE(Sprite)
    RESOURCE_TYPE(Sound)
    RESOURCE_TYPE(Background)
    RESOURCE_TYPE(Path)
    RESOURCE_TYPE(Script)
    RESOURCE_TYPE(Font)
    RESOURCE_TYPE(TimeLine)
    RESOURCE_TYPE(Object)
    RESOURCE_TYPE(Room)
#undef RESOURCE_TYPE
    qWarning() << "index is not in this project" << index;
    return NodeType::Root;
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

    auto [owner, node] = locationFor<T>(index);
    return owner && node ? node->resource() : nullptr;
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

    auto [owner, node] = locationFor<T>(index);
    return owner && node ? node->resource() : nullptr;
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

    const QModelIndex root = rootFor<T>();
    if (!insertRows(int(container.root.children.size()), 1, root)) return nullptr;
    return get<T>(index(int(container.root.children.size()) - 1, 0, root));
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
    auto [owner, node] = container.findNode(&entry);
    if (!owner || !node)
    {
        qWarning() << "entry not from this project!";
        return false;
    }

    return removeRows(owner->rowOf(node), 1, indexForResource(entry).parent());
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
    auto [owner, node] = container.findNode(&entry);
    if (!owner || !node || !node->resource())
    {
        qWarning() << "entry not from this project!";
        return false;
    }

    if (node->resource()->name == newName)
        return true;

    if (newName.trimmed().isEmpty()) return false;

    if (m_project->hasResourceName(newName) ||
        owner->children.template get<1>().find(newName) != owner->children.template get<1>().end())
    {
        qWarning() << "duplicate name" << newName;
        return false;
    }

    onBeforeRename<T>(*node->resource(), newName);

    const QString oldName = node->resource()->name;
    auto &sequence = owner->children.template get<0>();
    auto nodeIter = std::find_if(sequence.begin(), sequence.end(),
        [node](const auto &child) { return child.get() == node; });
    if (!sequence.modify(nodeIter, [&newName](auto &child) { child->resource()->name = newName; }))
        return false;
    container.invalidate();

    emitNameChanged<T>(*node->resource(), oldName);
    const auto index = indexForResource(*node->resource());
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    onAfterRename<T>(*node->resource(), oldName);

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

        const auto index = indexForResource(*iter);
        emit dataChanged(index, index, { Qt::DecorationRole });

        emit spritePixmapsChanged(*iter);
    }

    for (auto iter = std::cbegin(m_project->objects); iter != std::cend(m_project->objects); iter++)
    {
        if (iter->spriteName != sprite.name)
            continue;

        const auto index = indexForResource(*iter);
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

    const auto index = indexForResource(*iter);
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

    const auto index = indexForResource(*iter);
    emit dataChanged(index, index, { Qt::DecorationRole });

    emit objectSpriteNameChanged(object, std::move(oldSpriteName));

    return true;
}
