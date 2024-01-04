#include "projectserialization.h"

#include "stdserialization.h"

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

QDataStream &operator<<(QDataStream &ds, const MoveFixedAction &action)
{
    Q_UNUSED(action);
//   ds << action.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, MoveFixedAction &action)
{
    Q_UNUSED(action);
//    ds >> action.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const MoveFreeAction &action)
{
    Q_UNUSED(action);
//   ds << action.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, MoveFreeAction &action)
{
    Q_UNUSED(action);
//    ds >> action.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const MoveTowardsAction &action)
{
    Q_UNUSED(action);
//   ds << action.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, MoveTowardsAction &action)
{
    Q_UNUSED(action);
//    ds >> action.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ExecuteCodeAction &action)
{
    ds << action.script;
    ds << action.appliesTo;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, ExecuteCodeAction &action)
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
