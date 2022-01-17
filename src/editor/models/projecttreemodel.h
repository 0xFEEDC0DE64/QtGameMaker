#pragma once

#include <QAbstractItemModel>

#include <optional>

struct ProjectContainer;
struct Sprite;
struct Sound;
struct Background;
struct Path;
struct Script;
struct Font;
struct TimeLine;
struct Object;
struct Room;

class ProjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum class NodeType {
        Root = 12,
        Sprite,
        Sound,
        Background,
        Path,
        Script,
        Font,
        TimeLine,
        Object,
        Room,
    };

    explicit ProjectTreeModel(QObject *parent = nullptr);
    explicit ProjectTreeModel(ProjectContainer &project, QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    ProjectContainer *project() { return m_project; }
    const ProjectContainer *project() const { return m_project; }
    void setProject(ProjectContainer *project);

    NodeType nodeType(const QModelIndex &index) const;

    template<typename T> QModelIndex rootFor() const;

    QModelIndex gameInformationRoot() const;
    QModelIndex globalGameSettingsRoot() const;
    QModelIndex extensionPackagesRoot() const;

    template<typename T> T *get(const QModelIndex &index);
    template<typename T> const T *get(const QModelIndex &index) const;

    template<typename T> T *create();

    template<typename T> bool remove(const T &entry);

    template<typename T> bool rename(const T &entry, const QString &newName);

    template<typename T> static constexpr ProjectTreeModel::NodeType nodeTypeFor() = delete;

    bool setSpritePixmaps(const Sprite &sprite, std::vector<QPixmap> &&pixmaps);

    bool setBackgroundPixmap(const Background &background, QPixmap &&pixmap);

    bool setObjectSpriteName(const Object &object, QString &&spriteName);

signals:
    void errorOccured(const QString &message);

    void spriteCreated(const Sprite &sprite);
    void soundCreated(const Sound &sound);
    void backgroundCreated(const Background &background);
    void pathCreated(const Path &path);
    void scriptCreated(const Script &script);
    void fontCreated(const Font &font);
    void timeLineCreated(const TimeLine &timeLine);
    void objectCreated(const Object &object);
    void roomCreated(const Room &room);

    void spriteAboutToBeRemoved(const Sprite &sprite);
    void soundAboutToBeRemoved(const Sound &sound);
    void backgroundAboutToBeRemoved(const Background &background);
    void pathAboutToBeRemoved(const Path &path);
    void scriptAboutToBeRemoved(const Script &script);
    void fontAboutToBeRemoved(const Font &font);
    void timeLineAboutToBeRemoved(const TimeLine &timeLine);
    void objectAboutToBeRemoved(const Object &object);
    void roomAboutToBeRemoved(const Room &room);

    void spriteNameChanged(const Sprite &sprite, const QString &oldName);
    void soundNameChanged(const Sound &sound, const QString &oldName);
    void backgroundNameChanged(const Background &background, const QString &oldName);
    void pathNameChanged(const Path &path, const QString &oldName);
    void scriptNameChanged(const Script &script, const QString &oldName);
    void fontNameChanged(const Font &font, const QString &oldName);
    void timeLineNameChanged(const TimeLine &timeLine, const QString &oldName);
    void objectNameChanged(const Object &object, const QString &oldName);
    void roomNameChanged(const Room &room, const QString &oldName);

    void spritePixmapsChanged(const Sprite &sprite);

    void backgroundPixmapChanged(const Background &background);

    void objectSpriteNameChanged(const Object &object, const QString &oldSpriteName);

private:
    template<typename T> QVariant dataFor(const QModelIndex &index, int role) const;
    template<typename T> QVariant iconFor(const T &entry) const;
    template<typename T> bool setDataFor(const QModelIndex &index, const QVariant &value, int role);
    template<typename T> std::optional<bool> insertRowsFor(int row, int count, const QModelIndex &parent);
    template<typename T> std::optional<bool> removeRowsFor(int row, int count, const QModelIndex &parent);
    template<typename T> static QString getFreeNameFor(const std::list<T> &container);

    template<typename T> void emitCreated(const T &entry);
    template<typename T> void onBeforeRemove(const T &entry);
    template<typename T> void emitAboutToBeRemoved(const T &entry);
    template<typename T> void onBeforeRename(const T &entry, const QString &newName);
    template<typename T> void onAfterRename(const T &entry, const QString &oldName);
    template<typename T> void emitNameChanged(const T &entry, const QString &oldName);
    template<typename T> static QString nameTempateFor();

    ProjectContainer *m_project{};
};

template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Sprite>()     { return ProjectTreeModel::NodeType::Sprite; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Sound>()      { return ProjectTreeModel::NodeType::Sound; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Background>() { return ProjectTreeModel::NodeType::Background; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Path>()       { return ProjectTreeModel::NodeType::Path; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Script>()     { return ProjectTreeModel::NodeType::Script; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Font>()       { return ProjectTreeModel::NodeType::Font; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<TimeLine>()   { return ProjectTreeModel::NodeType::TimeLine; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Object>()     { return ProjectTreeModel::NodeType::Object; }
template<> constexpr ProjectTreeModel::NodeType ProjectTreeModel::nodeTypeFor<Room>()       { return ProjectTreeModel::NodeType::Room; }

template<> QModelIndex ProjectTreeModel::rootFor<Sprite>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Sound>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Background>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Path>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Script>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Font>() const;
template<> QModelIndex ProjectTreeModel::rootFor<TimeLine>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Object>() const;
template<> QModelIndex ProjectTreeModel::rootFor<Room>() const;
