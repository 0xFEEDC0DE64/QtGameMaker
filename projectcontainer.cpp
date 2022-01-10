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

QDataStream &operator<<(QDataStream &ds, const Sprite &sprite)
{
    ds << sprite.name;
    ds << sprite.pixmaps;
    ds << sprite.origin.x;
    ds << sprite.origin.y;
    ds << sprite.preciseCollisionChecking;
    ds << sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sprite &sprite)
{
    ds >> sprite.name;
    ds >> sprite.pixmaps;
    ds >> sprite.origin.x;
    ds >> sprite.origin.y;
    ds >> sprite.preciseCollisionChecking;
    ds >> sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Sound &sound)
{
    ds << sound.name;
    ds << sound.type;
    ds << sound.path;
    ds << sound.effects.chorus;
    ds << sound.effects.flanger;
    ds << sound.effects.gargle;
    ds << sound.effects.echo;
    ds << sound.effects.reverb;
    ds << sound.volume;
    ds << sound.pan;
    ds << sound.preload;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sound &sound)
{
    ds >> sound.name;
    ds >> sound.type;
    ds >> sound.path;
    ds >> sound.effects.chorus;
    ds >> sound.effects.flanger;
    ds >> sound.effects.gargle;
    ds >> sound.effects.echo;
    ds >> sound.effects.reverb;
    ds >> sound.volume;
    ds >> sound.pan;
    ds >> sound.preload;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Background &background)
{
    ds << background.name;
    ds << background.pixmap;
    ds << background.tileset;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Background &background)
{
    ds >> background.name;
    ds >> background.pixmap;
    ds >> background.tileset;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Path::Point &point)
{
    ds << point.point;
    ds << point.sp;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Path::Point &point)
{
    ds >> point.point;
    ds >> point.sp;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Path &path)
{
    ds << path.name;
    ds << path.points;
    ds << path.type;
    ds << path.closed;
    ds << path.precision;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Path &path)
{
    ds >> path.name;
    ds >> path.points;
    ds >> path.type;
    ds >> path.closed;
    ds >> path.precision;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Script &script)
{
    ds << script.name;
    ds << script.script;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Script &script)
{
    ds >> script.name;
    ds >> script.script;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Font &font)
{
    ds << font.name;
    ds << font.font;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Font &font)
{
    ds >> font.name;
    ds >> font.font;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const TimeLine &timeLine)
{
    ds << timeLine.name;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, TimeLine &timeLine)
{
    ds >> timeLine.name;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Object &object)
{
    ds << object.name;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Object &object)
{
    ds >> object.name;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Room &room)
{
    ds << room.name;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Room &room)
{
    ds >> room.name;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project)
{
    ds << project.sprites;
    ds << project.sounds;
    ds << project.backgrounds;
    ds << project.paths;
    ds << project.scripts;
    ds << project.fonts;
    ds << project.timeLines;
    ds << project.objects;
    ds << project.rooms;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, ProjectContainer &project)
{
    ds >> project.sprites;
    ds >> project.sounds;
    ds >> project.backgrounds;
    ds >> project.paths;
    ds >> project.scripts;
    ds >> project.fonts;
    ds >> project.timeLines;
    ds >> project.objects;
    ds >> project.rooms;
    return ds;
}

