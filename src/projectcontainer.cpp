#include "projectcontainer.h"

template<> std::list<Sprite> &ProjectContainer::containerFor() { return sprites; }
template<> const std::list<Sprite> &ProjectContainer::containerFor() const { return sprites; }
template<> std::list<Sound> &ProjectContainer::containerFor() { return sounds; }
template<> const std::list<Sound> &ProjectContainer::containerFor() const { return sounds; }
template<> std::list<Background> &ProjectContainer::containerFor() { return backgrounds; }
template<> const std::list<Background> &ProjectContainer::containerFor() const { return backgrounds; }
template<> std::list<Path> &ProjectContainer::containerFor() { return paths; }
template<> const std::list<Path> &ProjectContainer::containerFor() const { return paths; }
template<> std::list<Script> &ProjectContainer::containerFor() { return scripts; }
template<> const std::list<Script> &ProjectContainer::containerFor() const { return scripts; }
template<> std::list<Font> &ProjectContainer::containerFor() { return fonts; }
template<> const std::list<Font> &ProjectContainer::containerFor() const { return fonts; }
template<> std::list<TimeLine> &ProjectContainer::containerFor() { return timeLines; }
template<> const std::list<TimeLine> &ProjectContainer::containerFor() const { return timeLines; }
template<> std::list<Object> &ProjectContainer::containerFor() { return objects; }
template<> const std::list<Object> &ProjectContainer::containerFor() const { return objects; }
template<> std::list<Room> &ProjectContainer::containerFor() { return rooms; }
template<> const std::list<Room> &ProjectContainer::containerFor() const { return rooms; }

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

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings &globalGameSettings)
{
    Q_UNUSED(globalGameSettings);
    //ds << globalGameSettings.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings &globalGameSettings)
{
    Q_UNUSED(globalGameSettings);
    //ds >> globalGameSettings.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Sprite &sprite)
{
    ds << sprite.name
       << sprite.pixmaps
       << sprite.origin.x
       << sprite.origin.y
       << sprite.preciseCollisionChecking
       << sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sprite &sprite)
{
    ds >> sprite.name
       >> sprite.pixmaps
       >> sprite.origin.x
       >> sprite.origin.y
       >> sprite.preciseCollisionChecking
       >> sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Sound &sound)
{
    ds << sound.name
       << sound.type
       << sound.path
       << sound.effects.chorus
       << sound.effects.flanger
       << sound.effects.gargle
       << sound.effects.echo
       << sound.effects.reverb
       << sound.volume
       << sound.pan
       << sound.preload;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sound &sound)
{
    ds >> sound.name
       >> sound.type
       >> sound.path
       >> sound.effects.chorus
       >> sound.effects.flanger
       >> sound.effects.gargle
       >> sound.effects.echo
       >> sound.effects.reverb
       >> sound.volume
       >> sound.pan
       >> sound.preload;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Background &background)
{
    ds << background.name
       << background.pixmap
       << background.tileset;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Background &background)
{
    ds >> background.name
       >> background.pixmap
       >> background.tileset;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Path::Point &point)
{
    ds << point.point
       << point.sp;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Path::Point &point)
{
    ds >> point.point
       >> point.sp;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Path &path)
{
    ds << path.name
       << path.points
       << path.type
       << path.closed
       << path.precision
       << path.snapX
       << path.snapY
       << path.gridEnabled;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Path &path)
{
    ds >> path.name
       >> path.points
       >> path.type
       >> path.closed
       >> path.precision
       >> path.snapX
       >> path.snapY
       >> path.gridEnabled;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Script &script)
{
    ds << script.name
       << script.script;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Script &script)
{
    ds >> script.name
       >> script.script;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Font &font)
{
    ds << font.name
       << font.font;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Font &font)
{
    ds >> font.name
       >> font.font;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Action &action)
{
    ds << action.script;
    ds << action.appliesTo;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Action &action)
{
    ds >> action.script;
    ds >> action.appliesTo;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const TimeLine &timeLine)
{
    ds << timeLine.name
       << timeLine.moments;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, TimeLine &timeLine)
{
    ds >> timeLine.name
       >> timeLine.moments;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Object &object)
{
    ds << object.name
       << object.spriteName
       << object.visible
       << object.solid
       << object.depth
       << object.persistent
       << object.parentName
       << object.events
       << object.collisionEvents;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Object &object)
{
    ds >> object.name
       >> object.spriteName
       >> object.visible
       >> object.solid
       >> object.depth
       >> object.persistent
       >> object.parentName
       >> object.events
       >> object.collisionEvents;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Room::Object &object)
{
    ds << object.objectName
       << object.pos;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Room::Object &object)
{
    ds >> object.objectName
       >> object.pos;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Room &room)
{
    ds << room.name
       << room.caption
       << room.width
       << room.height
       << room.speed
       << room.persistent
       << room.creationCode
       << room.snapX
       << room.snapY
       << room.gridEnabled
       << room.isometricGrid
       << room.objects;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Room &room)
{
    ds >> room.name
       >> room.caption
       >> room.width
       >> room.height
       >> room.speed
       >> room.persistent
       >> room.creationCode
       >> room.snapX
       >> room.snapY
       >> room.gridEnabled
       >> room.isometricGrid
       >> room.objects;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project)
{
    ds << project.globalGameSettings
       << project.sprites
       << project.sounds
       << project.backgrounds
       << project.paths
       << project.scripts
       << project.fonts
       << project.timeLines
       << project.objects
       << project.rooms;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, ProjectContainer &project)
{
    ds >> project.globalGameSettings
       >> project.sprites
       >> project.sounds
       >> project.backgrounds
       >> project.paths
       >> project.scripts
       >> project.fonts
       >> project.timeLines
       >> project.objects
       >> project.rooms;
    return ds;
}

