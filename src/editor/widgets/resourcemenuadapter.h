#pragma once

#include <QMetaObject>
#include <QPersistentModelIndex>
#include <QList>

#include <functional>
#include <memory>
#include <vector>

class ProjectTreeModel;
class QMenu;
class QAction;

class ResourceMenuAdapter
{
public:
    using Selection = std::function<void(const QModelIndex &)>;
    using Prefix = std::function<void(QMenu &)>;

    ResourceMenuAdapter(ProjectTreeModel &model, const QModelIndex &root, QMenu &menu,
                        Selection selection, Prefix prefix = {});
    ~ResourceMenuAdapter();

    void rebuild();

private:
    struct Entry {
        QPersistentModelIndex index;
        QAction *action{};
        QMenu *submenu{};
        std::vector<std::unique_ptr<Entry>> children;
    };

    Entry *findEntry(const QModelIndex &index);
    std::vector<std::unique_ptr<Entry>> *entriesFor(const QModelIndex &parent);
    QMenu *menuFor(const QModelIndex &parent);
    void populate(const QModelIndex &parent, QMenu &menu, std::vector<std::unique_ptr<Entry>> &entries);
    void insertEntry(const QModelIndex &index, QMenu &menu,
                     std::vector<std::unique_ptr<Entry>> &entries, int row);
    void removeEntries(const QModelIndex &parent, int first, int last);
    void updateEntries(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                       const QList<int> &roles);
    void moveEntriesAboutTo(const QModelIndex &sourceParent, int sourceFirst, int sourceLast,
                            const QModelIndex &destinationParent, int destinationRow);
    void moveEntries(const QModelIndex &sourceParent, int sourceFirst, int sourceLast,
                     const QModelIndex &destinationParent, int destinationRow);
    static void destroyEntry(QMenu &menu, Entry &entry);

    ProjectTreeModel &m_model;
    QPersistentModelIndex m_root;
    int m_rootRow{};
    QMenu &m_menu;
    Selection m_selection;
    Prefix m_prefix;
    std::vector<QMetaObject::Connection> m_connections;
    std::vector<std::unique_ptr<Entry>> m_entries;
    std::vector<std::unique_ptr<Entry>> m_movingEntries;
};
