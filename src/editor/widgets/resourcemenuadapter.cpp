#include "resourcemenuadapter.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QMenu>
#include <QPixmap>

#include <iterator>

#include "models/projecttreemodel.h"

namespace {
QIcon iconFor(const ProjectTreeModel &model, const QModelIndex &index)
{
    const QVariant value = model.data(index, Qt::DecorationRole);
    if (value.metaType() == QMetaType::fromType<QPixmap>())
        return QIcon{value.value<QPixmap>()};
    return value.value<QIcon>();
}
}

ResourceMenuAdapter::ResourceMenuAdapter(ProjectTreeModel &model, const QModelIndex &root,
                                         QMenu &menu, Selection selection, Prefix prefix) :
    m_model{model}, m_root{root}, m_rootRow{root.row()}, m_menu{menu},
    m_selection{std::move(selection)}, m_prefix{std::move(prefix)}
{
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::rowsInserted, &menu,
        [this](const QModelIndex &parent, int first, int last) {
            auto *entries = entriesFor(parent);
            auto *targetMenu = menuFor(parent);
            if (!entries || !targetMenu) return;
            for (int row = first; row <= last; ++row)
                insertEntry(m_model.index(row, 0, parent), *targetMenu, *entries, row);
        }));
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::rowsAboutToBeRemoved, &menu,
        [this](const QModelIndex &parent, int first, int last) { removeEntries(parent, first, last); }));
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::rowsAboutToBeMoved, &menu,
        [this](const QModelIndex &source, int first, int last, const QModelIndex &destination, int row) {
            moveEntriesAboutTo(source, first, last, destination, row);
        }));
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::rowsMoved, &menu,
        [this](const QModelIndex &source, int first, int last, const QModelIndex &destination, int row) {
            moveEntries(source, first, last, destination, row);
        }));
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::dataChanged, &menu,
        [this](const QModelIndex &first, const QModelIndex &last, const QList<int> &roles) {
            updateEntries(first, last, roles);
        }));
    m_connections.push_back(QObject::connect(&model, &QAbstractItemModel::modelReset, &menu,
        [this] { rebuild(); }));
    rebuild();
}

ResourceMenuAdapter::~ResourceMenuAdapter()
{
    for (const auto &connection : m_connections)
        QObject::disconnect(connection);
    for (auto &entry : m_entries)
        destroyEntry(m_menu, *entry);
    m_entries.clear();
    m_menu.clear();
}

void ResourceMenuAdapter::rebuild()
{
    for (auto &entry : m_entries)
        destroyEntry(m_menu, *entry);
    m_entries.clear();
    m_menu.clear();
    if (m_prefix) m_prefix(m_menu);
    if (!m_root.isValid()) m_root = m_model.index(m_rootRow, 0, {});
    if (m_root.isValid()) populate(m_root, m_menu, m_entries);
}

ResourceMenuAdapter::Entry *ResourceMenuAdapter::findEntry(const QModelIndex &index)
{
    const auto find = [&](const auto &self, auto &entries) -> Entry * {
        for (auto &entry : entries)
        {
            if (entry->index == index) return entry.get();
            if (auto *child = self(self, entry->children)) return child;
        }
        return nullptr;
    };
    return find(find, m_entries);
}

std::vector<std::unique_ptr<ResourceMenuAdapter::Entry>> *
ResourceMenuAdapter::entriesFor(const QModelIndex &parent)
{
    if (parent == m_root) return &m_entries;
    if (auto *entry = findEntry(parent); entry && entry->submenu) return &entry->children;
    return nullptr;
}

QMenu *ResourceMenuAdapter::menuFor(const QModelIndex &parent)
{
    if (parent == m_root) return &m_menu;
    if (auto *entry = findEntry(parent)) return entry->submenu;
    return nullptr;
}

void ResourceMenuAdapter::populate(const QModelIndex &parent, QMenu &menu,
                                   std::vector<std::unique_ptr<Entry>> &entries)
{
    for (int row = 0; row < m_model.rowCount(parent); ++row)
        insertEntry(m_model.index(row, 0, parent), menu, entries, row);
}

void ResourceMenuAdapter::insertEntry(const QModelIndex &index, QMenu &menu,
                                      std::vector<std::unique_ptr<Entry>> &entries, int row)
{
    if (!index.isValid()) return;
    auto entry = std::make_unique<Entry>();
    entry->index = index;
    const QString name = m_model.data(index, Qt::DisplayRole).toString();
    const QIcon icon = iconFor(m_model, index);
    if (m_model.nodeType(index) == ProjectTreeModel::NodeType::Folder)
    {
        entry->submenu = new QMenu(name, &menu);
        entry->submenu->setIcon(icon);
        entry->action = entry->submenu->menuAction();
        populate(index, *entry->submenu, entry->children);
    }
    else
    {
        entry->action = new QAction(icon, name, &menu);
        Entry *selectedEntry = entry.get();
        QObject::connect(entry->action, &QAction::triggered, entry->action, [this, selectedEntry] {
            if (selectedEntry->index.isValid() && selectedEntry->index.model() == &m_model)
                m_selection(selectedEntry->index);
        });
    }
    const auto position = std::next(entries.begin(), row);
    menu.insertAction(position == entries.end() ? nullptr : (*position)->action, entry->action);
    entries.insert(position, std::move(entry));
}

void ResourceMenuAdapter::destroyEntry(QMenu &menu, Entry &entry)
{
    menu.removeAction(entry.action);
    if (entry.submenu) delete entry.submenu;
    else delete entry.action;
}

void ResourceMenuAdapter::removeEntries(const QModelIndex &parent, int first, int last)
{
    auto *entries = entriesFor(parent);
    auto *menu = menuFor(parent);
    if (!entries || !menu) return;
    for (int row = last; row >= first; --row)
    {
        destroyEntry(*menu, *entries->at(row));
        entries->erase(std::next(entries->begin(), row));
    }
}

void ResourceMenuAdapter::updateEntries(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                        const QList<int> &roles)
{
    const bool updateText = roles.empty() || roles.contains(Qt::DisplayRole);
    const bool updateIcon = roles.empty() || roles.contains(Qt::DecorationRole);
    if (!updateText && !updateIcon) return;
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
        const QModelIndex index = m_model.index(row, 0, topLeft.parent());
        auto *entry = findEntry(index);
        if (!entry) continue;
        if (updateText)
        {
            const QString name = m_model.data(index, Qt::DisplayRole).toString();
            if (entry->submenu) entry->submenu->setTitle(name);
            else entry->action->setText(name);
        }
        if (updateIcon)
        {
            const QIcon icon = iconFor(m_model, index);
            if (entry->submenu) entry->submenu->setIcon(icon);
            else entry->action->setIcon(icon);
        }
    }
}

void ResourceMenuAdapter::moveEntriesAboutTo(const QModelIndex &sourceParent, int sourceFirst, int sourceLast,
                                             const QModelIndex &, int)
{
    auto *entries = entriesFor(sourceParent);
    auto *menu = menuFor(sourceParent);
    if (!entries || !menu) return;
    for (int row = sourceFirst; row <= sourceLast; ++row)
    {
        auto &entry = entries->at(row);
        menu->removeAction(entry->action);
        m_movingEntries.push_back(std::move(entry));
    }
    entries->erase(std::next(entries->begin(), sourceFirst), std::next(entries->begin(), sourceLast + 1));
}

void ResourceMenuAdapter::moveEntries(const QModelIndex &sourceParent, int sourceFirst, int sourceLast,
                                      const QModelIndex &destinationParent, int destinationRow)
{
    auto *entries = entriesFor(destinationParent);
    auto *menu = menuFor(destinationParent);
    if (!entries || !menu)
    {
        for (auto &entry : m_movingEntries)
        {
            if (entry->submenu) delete entry->submenu;
            else delete entry->action;
        }
        m_movingEntries.clear();
        return;
    }
    // Qt reports the destination row before removing the source rows.
    if (sourceParent == destinationParent && destinationRow > sourceFirst)
        destinationRow -= sourceLast - sourceFirst + 1;
    for (int row = sourceFirst; row <= sourceLast; ++row)
    {
        if (m_movingEntries.empty())
        {
            insertEntry(m_model.index(destinationRow, 0, destinationParent), *menu, *entries,
                        destinationRow);
        }
        else
        {
            auto entry = std::move(m_movingEntries.front());
            m_movingEntries.erase(m_movingEntries.begin());
            if (entry->submenu) entry->submenu->setParent(menu);
            else entry->action->setParent(menu);
            const auto position = std::next(entries->begin(), destinationRow);
            menu->insertAction(position == entries->end() ? nullptr : (*position)->action, entry->action);
            entries->insert(position, std::move(entry));
        }
        ++destinationRow;
    }
}
