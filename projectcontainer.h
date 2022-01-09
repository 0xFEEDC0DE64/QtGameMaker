#pragma once

#include <vector>
#include <list>

#include <QString>
#include <QPixmap>
#include <QFont>
#include <QDataStream>

struct Sprite
{
    QString name;
    std::vector<QPixmap> pixmaps;
    struct {
        int x{};
        int y{};
    } origin;
    bool preciseCollisionChecking{true};
    bool separateCollisionMasks{true};
};

struct Sound
{
    QString name;
    enum class Type { Sound, Music };
    Type type;
    QString path;
    struct {
        bool chorus{};
        bool flanger{};
        bool gargle{};
        bool echo{};
        bool reverb{};
    } effects;

    int volume{100};
    int pan{};
    bool preload{true};
};

struct Background
{
    QString name;
    QPixmap pixmap;
    bool tileset{};
};

struct Path
{
    QString name;
};

struct Script
{
    QString name;
    QString script;
};

struct Font
{
    QString name;
    QFont font;
    struct {
        int from{32};
        int to{127};
    } range;
};

struct ProjectContainer
{
    std::list<Sprite> sprites;
    std::list<Sound> sounds;    
    std::list<Background> backgrounds;
    std::list<Path> paths;
    std::list<Script> scripts;
    std::list<Font> fonts;
};

QDataStream &operator<<(QDataStream &ds, const Sprite &sprite);
QDataStream &operator>>(QDataStream &ds, Sprite &sprite);
QDataStream &operator<<(QDataStream &ds, const Sound &sound);
QDataStream &operator>>(QDataStream &ds, Sound &sound);
QDataStream &operator<<(QDataStream &ds, const Background &background);
QDataStream &operator>>(QDataStream &ds, Background &background);
QDataStream &operator<<(QDataStream &ds, const Path &path);
QDataStream &operator>>(QDataStream &ds, Path &path);
QDataStream &operator<<(QDataStream &ds, const Script &script);
QDataStream &operator>>(QDataStream &ds, Script &script);
QDataStream &operator<<(QDataStream &ds, const Font &font);
QDataStream &operator>>(QDataStream &ds, Font &font);
QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project);
QDataStream &operator>>(QDataStream &ds, ProjectContainer &project);
