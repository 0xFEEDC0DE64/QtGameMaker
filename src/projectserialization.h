#pragma once

#include <QDataStream>

#include "projectcontainer.h"

QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings &globalGameSettings);
QDataStream &operator>>(QDataStream &ds, GlobalGameSettings &globalGameSettings);
QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::FixedScale &fixedScale);
QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::FixedScale &fixedScale);
QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::KeepAspectRatio &keepAspectRatio);
QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::KeepAspectRatio &keepAspectRatio);
QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::FullScale &fullScale);
QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::FullScale &fullScale);
QDataStream &operator<<(QDataStream &ds, const GlobalGameSettings::Resolution &resolution);
QDataStream &operator>>(QDataStream &ds, GlobalGameSettings::Resolution &resolution);
QDataStream &operator<<(QDataStream &ds, const Sprite &sprite);
QDataStream &operator>>(QDataStream &ds, Sprite &sprite);
QDataStream &operator<<(QDataStream &ds, const Sound &sound);
QDataStream &operator>>(QDataStream &ds, Sound &sound);
QDataStream &operator<<(QDataStream &ds, const Background &background);
QDataStream &operator>>(QDataStream &ds, Background &background);
QDataStream &operator<<(QDataStream &ds, const Path::Point &point);
QDataStream &operator>>(QDataStream &ds, Path::Point &point);
QDataStream &operator<<(QDataStream &ds, const Path &path);
QDataStream &operator>>(QDataStream &ds, Path &path);
QDataStream &operator<<(QDataStream &ds, const Script &script);
QDataStream &operator>>(QDataStream &ds, Script &script);
QDataStream &operator<<(QDataStream &ds, const Font &font);
QDataStream &operator>>(QDataStream &ds, Font &font);
QDataStream &operator<<(QDataStream &ds, const MoveFixedAction &action);
QDataStream &operator>>(QDataStream &ds, MoveFixedAction &action);
QDataStream &operator<<(QDataStream &ds, const MoveFreeAction &action);
QDataStream &operator>>(QDataStream &ds, MoveFreeAction &action);
QDataStream &operator<<(QDataStream &ds, const MoveTowardsAction &action);
QDataStream &operator>>(QDataStream &ds, MoveTowardsAction &action);
QDataStream &operator<<(QDataStream &ds, const ExecuteCodeAction &action);
QDataStream &operator>>(QDataStream &ds, ExecuteCodeAction &action);
QDataStream &operator<<(QDataStream &ds, const TimeLine &timeLine);
QDataStream &operator>>(QDataStream &ds, TimeLine &timeLine);
QDataStream &operator<<(QDataStream &ds, const Object &object);
QDataStream &operator>>(QDataStream &ds, Object &object);
QDataStream &operator<<(QDataStream &ds, const Room::Object &object);
QDataStream &operator>>(QDataStream &ds, Room::Object &object);
QDataStream &operator<<(QDataStream &ds, const Room &room);
QDataStream &operator>>(QDataStream &ds, Room &room);
QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project);;
QDataStream &operator>>(QDataStream &ds, ProjectContainer &project);;
