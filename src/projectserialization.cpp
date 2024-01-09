#include "projectserialization.h"

#include "stdserialization.h"

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings &globalGameSettings)
{
    ds << globalGameSettings.startInFullscreenMode
       << globalGameSettings.scaling
       << globalGameSettings.interpolateBetweenColors
       << globalGameSettings.colorOutsideRoomRegion
       << globalGameSettings.resizable
       << globalGameSettings.topMost
       << globalGameSettings.noBorder
       << globalGameSettings.noButtons
       << globalGameSettings.displayCursor
       << globalGameSettings.freezeWhenFocusLost
       << globalGameSettings.disableScreensaver
       << globalGameSettings.screenSetting
       << globalGameSettings.vsync
       << globalGameSettings.letEscEndGame
       << globalGameSettings.treatCloseButtonAsEsc
       << globalGameSettings.letF1ShowGameInformation
       << globalGameSettings.letF4SwitchBetweenScreenModes
       << globalGameSettings.letF5SaveF6LoadGame
       << globalGameSettings.letF9TakeScreenshot
       << globalGameSettings.priority
       << globalGameSettings.major
       << globalGameSettings.minor
       << globalGameSettings.release
       << globalGameSettings.build
       << globalGameSettings.company
       << globalGameSettings.product
       << globalGameSettings.copyright
       << globalGameSettings.description;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings &globalGameSettings)
{
    ds >> globalGameSettings.startInFullscreenMode
       >> globalGameSettings.scaling
       >> globalGameSettings.interpolateBetweenColors
       >> globalGameSettings.colorOutsideRoomRegion
       >> globalGameSettings.resizable
       >> globalGameSettings.topMost
       >> globalGameSettings.noBorder
       >> globalGameSettings.noButtons
       >> globalGameSettings.displayCursor
       >> globalGameSettings.freezeWhenFocusLost
       >> globalGameSettings.disableScreensaver
       >> globalGameSettings.screenSetting
       >> globalGameSettings.vsync
       >> globalGameSettings.letEscEndGame
       >> globalGameSettings.treatCloseButtonAsEsc
       >> globalGameSettings.letF1ShowGameInformation
       >> globalGameSettings.letF4SwitchBetweenScreenModes
       >> globalGameSettings.letF5SaveF6LoadGame
       >> globalGameSettings.letF9TakeScreenshot
       >> globalGameSettings.priority
       >> globalGameSettings.major
       >> globalGameSettings.minor
       >> globalGameSettings.release
       >> globalGameSettings.build
       >> globalGameSettings.company
       >> globalGameSettings.product
       >> globalGameSettings.copyright
       >> globalGameSettings.description;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::FixedScale &fixedScale)
{
    ds << fixedScale.percent;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::FixedScale &fixedScale)
{
    ds >> fixedScale.percent;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::KeepAspectRatio &keepAspectRatio)
{
    Q_UNUSED(keepAspectRatio);
    //ds << keepAspectRatio.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::KeepAspectRatio &keepAspectRatio)
{
    Q_UNUSED(keepAspectRatio);
    //ds >> keepAspectRatio.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::FullScale &fullScale)
{
    Q_UNUSED(fullScale);
    //ds << fullScale.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::FullScale &fullScale)
{
    Q_UNUSED(fullScale);
    //ds >> fullScale.;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::ScreenSetting &screenSetting)
{
    ds << screenSetting.colorDepth
       << screenSetting.resolution
       << screenSetting.frequency;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::ScreenSetting &screenSetting)
{
    ds >> screenSetting.colorDepth
       >> screenSetting.resolution
       >> screenSetting.frequency;
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
       << object.maskSpriteName
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
       >> object.maskSpriteName
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

QDataStream &operator<<(QDataStream &ds, const IncludedFile &includedFile)
{
    Q_UNUSED(includedFile);
    //ds << includedFile.;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, IncludedFile &includedFile)
{
    Q_UNUSED(includedFile);
    //ds >> includedFile.;
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
       << project.rooms
       << project.includedFiles;
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
       >> project.rooms
       >> project.includedFiles;
    return ds;
}
