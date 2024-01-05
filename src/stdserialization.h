#pragma once

#include <array>
#include <list>
#include <vector>
#include <map>
#include <variant>
#include <optional>

#include <QDataStream>

template<typename T, std::size_t Tsize>
QDataStream &operator<<(QDataStream &ds, const std::array<T, Tsize> &array)
{
    for (const auto &entry : array)
        ds << entry;
    return ds;
}

template<typename T, std::size_t Tsize>
QDataStream &operator>>(QDataStream &ds, std::array<T, Tsize> &array)
{
    for (size_t i = 0; i < Tsize; i++)
        ds >> array[i];
    return ds;
}

template<typename T>
QDataStream &operator<<(QDataStream &ds, const std::list<T> &list)
{
    {
        int entries = list.size();
        ds << entries;
    }
    for (const auto &entry : list)
        ds << entry;
    return ds;
}

template<typename T>
QDataStream &operator>>(QDataStream &ds, std::list<T> &list)
{
    int entries;
    ds >> entries;

    for (int i = 0; i < entries; i++)
    {
        T entry;
        ds >> entry;
        list.emplace_back(std::move(entry));
    }
    return ds;
}

template<typename T>
QDataStream &operator<<(QDataStream &ds, const std::vector<T> &list)
{
    {
        int entries = list.size();
        ds << entries;
    }
    for (const auto &entry : list)
        ds << entry;
    return ds;
}

template<typename T>
QDataStream &operator>>(QDataStream &ds, std::vector<T> &list)
{
    int entries;
    ds >> entries;

    for (int i = 0; i < entries; i++)
    {
        T entry;
        ds >> entry;
        list.emplace_back(std::move(entry));
    }
    return ds;
}

template<typename Tkey, typename Tvalue>
QDataStream &operator<<(QDataStream &ds, const std::map<Tkey, Tvalue> &map)
{
    {
        int entries = map.size();
        ds << entries;
    }
    for (auto iter = std::cbegin(map); iter != std::cend(map); iter++)
        ds << iter->first << iter->second;
    return ds;
}

template<typename Tkey, typename Tvalue>
QDataStream &operator>>(QDataStream &ds, std::map<Tkey, Tvalue> &map)
{
    int entries;
    ds >> entries;

    for (int i = 0; i < entries; i++)
    {
        Tkey key;
        Tvalue value;
        ds >> key
           >> value;
        map[std::move(key)] = std::move(value);
    }
    return ds;
}

template<typename ...T>
QDataStream &operator<<(QDataStream &ds, const std::variant<T...> &variant)
{
    int index = variant.index();
    ds << index;

    using func_t = void(QDataStream&, const std::variant<T...> &);
    static constexpr func_t *funcs[] = {
        [](QDataStream& ds, const std::variant<T...> &variant) {
            ds << std::get<T>(variant);
        }...
    };

    funcs[index](ds, variant);

    return ds;
}

namespace detail {
// for idiotic GCC we cannot use the usual lambda syntax but instead
// have to provide a template method, GCC sucks
template<typename T, typename ...Tvariant>
std::variant<Tvariant...> variantUnpacker(QDataStream& ds)
{
    T value;
    ds >> value;
    return value;
}
}

template<typename ...T>
QDataStream &operator>>(QDataStream &ds, std::variant<T...> &variant)
{
    int index;
    ds >> index;

    using func_t = std::variant<T...> (QDataStream&);
    static constexpr func_t *funcs[] = {
        detail::variantUnpacker<T, T...>...
    };
    variant = funcs[index](ds);

    return ds;
}

template<typename T>
QDataStream &operator<<(QDataStream &ds, const std::optional<T> &optional)
{
    {
        bool hasValue = optional.has_value();
        ds << hasValue;
    }
    if (optional)
        ds << *optional;
    return ds;
}

template<typename T>
QDataStream &operator>>(QDataStream &ds, std::optional<T> &optional)
{
    bool hasValue;
    ds >> hasValue;

    if (hasValue)
    {
        T entry;
        ds >> entry;
        optional = std::move(entry);
    }
    else
        optional = std::nullopt;

    return ds;
}
