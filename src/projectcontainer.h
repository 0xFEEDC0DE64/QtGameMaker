#pragma once

#include <vector>
#include <list>
#include <map>

#include <QString>
#include <QPixmap>
#include <QFont>
#include <QDataStream>
#include <QPoint>

struct GlobalGameSettings
{
};

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
    int snapX{16};
    int snapY{16};
    bool gridEnabled{true};
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

struct Action {
    enum class AppliesTo {
        Self,
        Other,
        Object
    };

    QString script;
    AppliesTo appliesTo{AppliesTo::Self};
};

using ActionsContainer = std::array<Action, 1>;

struct TimeLine
{
    using moment_key_t = int;
    using moments_container_t = std::map<moment_key_t, ActionsContainer>;

    QString name;

    moments_container_t moments;
};

struct Object
{
    enum class EventType {
        Create,
        Destroy,
        Alarm0,
        Alarm1,
        Alarm2,
        Alarm3,
        Alarm4,
        Alarm5,
        Alarm6,
        Alarm7,
        Alarm8,
        Alarm9,
        Alarm10,
        Alarm11,
        Step,
        BeginStep,
        EndStep,
        Draw,
    };

    using events_container_t = std::map<EventType, ActionsContainer>;
    using collision_events_container_t = std::map<QString, ActionsContainer>;

    QString name;
    QString spriteName;
    bool visible{true};
    bool solid{};
    int depth{};
    bool persistent{};
    QString parentName;
    events_container_t events;
    collision_events_container_t collisionEvents;
};

struct Room
{
    QString name;
    QString caption;
    int width{640};
    int height{480};
    int speed{30};
    bool persistent{};
    QString creationCode;
    int snapX{16};
    int snapY{16};
    bool gridEnabled{true};
    bool isometricGrid{};

    struct Object {
        QString objectName;
        QPoint pos;
    };

    std::vector<Object> objects;
};

struct ProjectContainer
{
    GlobalGameSettings globalGameSettings;
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
