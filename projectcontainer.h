#pragma once

#include <vector>
#include <list>

#include <QString>
#include <QPixmap>
#include <QFont>
#include <QDataStream>
#include <QPoint>

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
    Type type{Type::Sound};
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
    struct Point {
        QPoint point;
        int sp{100};
    };
    std::vector<Point> points;
    enum class Type { Straight, Smooth };
    Type type{Type::Straight};
    bool closed{true};
    int precision{4};
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

struct TimeLine
{
    QString name;
};

struct Object
{
    QString name;
};

struct Room
{
    QString name;
};

struct ProjectContainer
{
    std::list<Sprite> sprites;
    std::list<Sound> sounds;
    std::list<Background> backgrounds;
    std::list<Path> paths;
    std::list<Script> scripts;
    std::list<Font> fonts;
    std::list<TimeLine> timeLines;
    std::list<Object> objects;
    std::list<Room> rooms;

    template<typename T> std::list<T> &containerFor();
    template<typename T> const std::list<T> &containerFor() const;
};

QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project);
QDataStream &operator>>(QDataStream &ds, ProjectContainer &project);
