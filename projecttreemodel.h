#pragma once

#include <QAbstractItemModel>

struct ProjectContainer;
struct Sprite;
struct Sound;
struct Background;
struct Path;
struct Script;
struct Font;

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
    };

    explicit ProjectTreeModel(ProjectContainer *project, QObject *parent = nullptr);

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

    QModelIndex spritesRoot() const;
    QModelIndex soundsRoot() const;
    QModelIndex backgroundsRoot() const;
    QModelIndex pathsRoot() const;
    QModelIndex scriptsRoot() const;
    QModelIndex fontsRoot() const;
    QModelIndex timeLinesRoot() const;
    QModelIndex objectsRoot() const;
    QModelIndex roomsRoot() const;
    QModelIndex gameInformationRoot() const;
    QModelIndex globalGameSettingsRoot() const;
    QModelIndex extensionPackagesRoot() const;

    Sprite *getSprite(const QModelIndex &index);
    const Sprite *getSprite(const QModelIndex &index) const;

    Sound *getSound(const QModelIndex &index);
    const Sound *getSound(const QModelIndex &index) const;

    Background *getBackground(const QModelIndex &index);
    const Background *getBackground(const QModelIndex &index) const;

    Path *getPath(const QModelIndex &index);
    const Path *getPath(const QModelIndex &index) const;

    Script *getScript(const QModelIndex &index);
    const Script *getScript(const QModelIndex &index) const;

    Font *getFont(const QModelIndex &index);
    const Font *getFont(const QModelIndex &index) const;

private:
    ProjectContainer *m_project{};
};
