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
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;

    void setProject(ProjectContainer *project);
    NodeType nodeType(const QModelIndex &index) const;

    template<typename T> QModelIndex rootFor() const;

    QModelIndex gameInformationRoot() const;
    QModelIndex globalGameSettingsRoot() const;
    QModelIndex extensionPackagesRoot() const;

    template<typename T> T *get(const QModelIndex &index);
    template<typename T> const T *get(const QModelIndex &index) const;

    template<typename T> bool rename(const T &sprite, const QString &newName);

    template<typename T> static constexpr ProjectTreeModel::NodeType nodeTypeFor() = delete;

signals:
    void errorOccured(const QString &message);

    void spriteNameChanged(const Sprite &sprite);
    void soundNameChanged(const Sound &sound);
    void backgroundNameChanged(const Background &background);
    void pathNameChanged(const Path &path);
    void scriptNameChanged(const Script &script);
    void fontNameChanged(const Font &font);
    void timeLineNameChanged(const TimeLine &font);
    void objectNameChanged(const Object &font);
    void roomNameChanged(const Room &font);

private:
    template<typename T> QVariant dataFor(const QModelIndex &index, int role) const;
    template<typename T> QVariant iconFor(const T &entry) const;
    template<typename T> bool setDataFor(const QModelIndex &index, const QVariant &value, int role);
    template<typename T> std::optional<bool> insertRowsFor(int row, int count, const QModelIndex &parent);
    template<typename T> std::optional<bool> removeRowsFor(int row, int count, const QModelIndex &parent);
    template<typename T> void emitNameChanged(const T &entry);
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

//template<> Sprite *ProjectTreeModel::get<Sprite>(const QModelIndex &index);
//template<> const Sprite *ProjectTreeModel::get<Sprite>(const QModelIndex &index) const;
//template<> Sound *ProjectTreeModel::get<Sound>(const QModelIndex &index);
//template<> const Sound *ProjectTreeModel::get<Sound>(const QModelIndex &index) const;
//template<> Background *ProjectTreeModel::get<Background>(const QModelIndex &index);
//template<> const Background *ProjectTreeModel::get<Background>(const QModelIndex &index) const;
//template<> Path *ProjectTreeModel::get<Path>(const QModelIndex &index);
//template<> const Path *ProjectTreeModel::get<Path>(const QModelIndex &index) const;
//template<> Script *ProjectTreeModel::get<Script>(const QModelIndex &index);
//template<> const Script *ProjectTreeModel::get<Script>(const QModelIndex &index) const;
//template<> Font *ProjectTreeModel::get<Font>(const QModelIndex &index);
//template<> const Font *ProjectTreeModel::get<Font>(const QModelIndex &index) const;
//template<> TimeLine *ProjectTreeModel::get<TimeLine>(const QModelIndex &index);
//template<> const TimeLine *ProjectTreeModel::get<TimeLine>(const QModelIndex &index) const;
//template<> Object *ProjectTreeModel::get<Object>(const QModelIndex &index);
//template<> const Object *ProjectTreeModel::get<Object>(const QModelIndex &index) const;
//template<> Room *ProjectTreeModel::get<Room>(const QModelIndex &index);
//template<> const Room *ProjectTreeModel::get<Room>(const QModelIndex &index) const;

//template<> bool ProjectTreeModel::rename<Sprite>(const Sprite &sprite, const QString &newName);
//template<> bool ProjectTreeModel::rename<Sound>(const Sound &sound, const QString &newName);
//template<> bool ProjectTreeModel::rename<Background>(const Background &background, const QString &newName);
//template<> bool ProjectTreeModel::rename<Path>(const Path &path, const QString &newName);
//template<> bool ProjectTreeModel::rename<Script>(const Script &script, const QString &newName);
//template<> bool ProjectTreeModel::rename<Font>(const Font &font, const QString &newName);
//template<> bool ProjectTreeModel::rename<TimeLine>(const TimeLine &timeLine, const QString &newName);
//template<> bool ProjectTreeModel::rename<Object>(const Object &object, const QString &newName);
//template<> bool ProjectTreeModel::rename<Room>(const Room &room, const QString &newName);
