#pragma once

#include <QString>
#include <QHash>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/key.hpp>

#include <list>
#include <memory>
#include <variant>
#include <functional>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <iterator>

struct ResourceQStringHash {
    std::size_t operator()(const QString &name) const { return qHash(name); }
};

template<typename T> struct ResourceFolder;

template<typename T>
struct ResourceNode
{
    std::variant<T, std::unique_ptr<ResourceFolder<T>>> value;

    explicit ResourceNode(T resource) : value{std::move(resource)} {}
    explicit ResourceNode(std::unique_ptr<ResourceFolder<T>> folder) : value{std::move(folder)} {}

    bool isFolder() const { return std::holds_alternative<std::unique_ptr<ResourceFolder<T>>>(value); }
    QString name() const {
        return isFolder() ? std::get<std::unique_ptr<ResourceFolder<T>>>(value)->name
                          : std::get<T>(value).name;
    }
    T *resource() { return std::get_if<T>(&value); }
    const T *resource() const { return std::get_if<T>(&value); }
    ResourceFolder<T> *folder() {
        return isFolder() ? std::get<std::unique_ptr<ResourceFolder<T>>>(value).get() : nullptr;
    }
    const ResourceFolder<T> *folder() const {
        return isFolder() ? std::get<std::unique_ptr<ResourceFolder<T>>>(value).get() : nullptr;
    }
};

template<typename T>
struct ResourceNodeName
{
    using result_type = QString;
    QString operator()(const std::unique_ptr<ResourceNode<T>> &node) const { return node->name(); }
};

template<typename T>
struct ResourceFolder
{
    using Node = ResourceNode<T>;
    using Children = boost::multi_index::multi_index_container<
        std::unique_ptr<Node>,
        boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,
            boost::multi_index::hashed_unique<ResourceNodeName<T>, ResourceQStringHash>>>;

    QString name;
    Children children;

    explicit ResourceFolder(QString name = {}) : name{std::move(name)} {}

    Node *at(std::size_t row) const {
        if (row >= children.size()) return nullptr;
        return std::next(children.template get<0>().begin(), row)->get();
    }
    int rowOf(const Node *node) const {
        int row = 0;
        for (const auto &child : children.template get<0>()) {
            if (child.get() == node) return row;
            ++row;
        }
        return -1;
    }
};

// The folder tree owns every resource. The flat view keeps legacy editor and
// engine traversal working while callers are migrated to folder-aware APIs.
template<typename T>
class ResourceCollection
{
    using Node = ResourceNode<T>;
    using Folder = ResourceFolder<T>;
    mutable std::list<T*> m_flat;
    mutable QHash<QString, T*> m_byName;
    mutable bool m_dirty{true};

    static void collect(Folder &folder, std::list<T*> &flat) {
        for (const auto &node : folder.children.template get<0>()) {
            if (auto *resource = node->resource()) flat.push_back(resource);
            else collect(*node->folder(), flat);
        }
    }
    void ensureFlat() const {
        if (!m_dirty) return;
        m_flat.clear();
        m_byName.clear();
        collect(const_cast<Folder &>(root), m_flat);
        for (T *resource : m_flat) m_byName.insert(resource->name, resource);
        m_dirty = false;
    }
    static bool findNodeIn(Folder &folder, const T *resource, Folder *&owner, Node *&node) {
        for (const auto &child : folder.children.template get<0>()) {
            if (child->resource() == resource) { owner = &folder; node = child.get(); return true; }
            if (auto *nested = child->folder(); nested && findNodeIn(*nested, resource, owner, node)) return true;
        }
        return false;
    }
public:
    template<bool IsConst>
    class FlatIterator
    {
        using Base = typename std::list<T*>::const_iterator;
        Base m_position;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<IsConst, const T &, T &>;
        using pointer = std::conditional_t<IsConst, const T *, T *>;

        FlatIterator() = default;
        explicit FlatIterator(Base position) : m_position{position} {}
        reference operator*() const { return **m_position; }
        pointer operator->() const { return *m_position; }
        FlatIterator &operator++() { ++m_position; return *this; }
        FlatIterator operator++(int) { auto copy = *this; ++*this; return copy; }
        FlatIterator &operator--() { --m_position; return *this; }
        FlatIterator operator--(int) { auto copy = *this; --*this; return copy; }
        template<bool Other>
        bool operator==(const FlatIterator<Other> &other) const { return m_position == other.base(); }
        Base base() const { return m_position; }
    };
    using iterator = FlatIterator<false>;
    using const_iterator = FlatIterator<true>;
    Folder root;

    ResourceCollection() = default;
    ResourceCollection(ResourceCollection &&other) noexcept : root{std::move(other.root)}, m_dirty{true} {}
    ResourceCollection &operator=(ResourceCollection &&other) noexcept {
        root = std::move(other.root); m_flat.clear(); m_byName.clear(); m_dirty = true; return *this;
    }
    ResourceCollection(const ResourceCollection &) = delete;
    ResourceCollection &operator=(const ResourceCollection &) = delete;

    iterator begin() { ensureFlat(); return iterator{m_flat.cbegin()}; }
    iterator end() { ensureFlat(); return iterator{m_flat.cend()}; }
    const_iterator begin() const { ensureFlat(); return const_iterator{m_flat.cbegin()}; }
    const_iterator end() const { ensureFlat(); return const_iterator{m_flat.cend()}; }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    std::size_t size() const { ensureFlat(); return m_flat.size(); }
    bool empty() const { return size() == 0; }
    T &front() { return *begin(); }
    const T &front() const { return *begin(); }
    T &back() { auto it = end(); return *--it; }
    const T &back() const { auto it = end(); return *--it; }

    void invalidate() { m_dirty = true; }
    std::pair<Folder*, Node*> findNode(const T *resource) {
        Folder *owner{}; Node *node{};
        findNodeIn(root, resource, owner, node);
        return {owner, node};
    }
    T *find(const QString &name) {
        ensureFlat();
        return m_byName.value(name, nullptr);
    }
    const T *find(const QString &name) const {
        ensureFlat();
        return m_byName.value(name, nullptr);
    }

    iterator emplace(iterator position, T resource) {
        T *before = position == end() ? nullptr : &*position;
        auto &sequence = root.children.template get<0>();
        auto at = sequence.end();
        if (before) {
            for (auto it = sequence.begin(); it != sequence.end(); ++it)
                if ((*it)->resource() == before) { at = it; break; }
        }
        auto node = std::make_unique<Node>(std::move(resource));
        T *pointer = node->resource();
        sequence.insert(at, std::move(node));
        invalidate();
        for (auto it = begin(); it != end(); ++it) if (&*it == pointer) return it;
        return end();
    }
    void emplace_back(T resource) { emplace(end(), std::move(resource)); }
    iterator erase(iterator position) {
        if (position == end()) return end();
        T *pointer = &*position;
        auto next = std::next(position);
        T *nextPointer = next == end() ? nullptr : &*next;
        auto [owner, node] = findNode(pointer);
        if (owner && node) {
            auto &sequence = owner->children.template get<0>();
            auto it = std::find_if(sequence.begin(), sequence.end(),
                [node](const auto &child) { return child.get() == node; });
            sequence.erase(it);
            invalidate();
        }
        if (nextPointer)
            for (auto it = begin(); it != end(); ++it) if (&*it == nextPointer) return it;
        return end();
    }
    iterator erase(iterator first, iterator last) {
        std::vector<T*> removed;
        for (auto it = first; it != last; ++it) removed.push_back(&*it);
        T *nextPointer = last == end() ? nullptr : &*last;
        for (T *pointer : removed) {
            auto [owner, node] = findNode(pointer);
            if (!owner || !node) continue;
            auto &sequence = owner->children.template get<0>();
            auto it = std::find_if(sequence.begin(), sequence.end(),
                [node](const auto &child) { return child.get() == node; });
            sequence.erase(it);
        }
        invalidate();
        if (nextPointer)
            for (auto it = begin(); it != end(); ++it) if (&*it == nextPointer) return it;
        return end();
    }
    void clear() { root.children.clear(); invalidate(); }
};
