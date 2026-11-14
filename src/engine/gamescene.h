#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include <QImage>
#include <QPointF>
#include <QRectF>
#include <QSize>
#include <QString>
#include <Qt>

#include "projectcontainer.h"

struct GameScene;
struct ObjectInstance;

// The interpreter supplies action behavior. `other` is the collision counterpart
// for self actions, or the event source when an action targets another instance.
using ActionExecutor = std::function<void(ObjectInstance &target, const Action &action,
                                          ObjectInstance *other)>;

struct ObjectInstance
{
    ObjectInstance() { alarms.fill(-1); }

    const Object *definition{};
    std::optional<std::size_t> spriteIndex;
    std::optional<std::size_t> maskSpriteIndex;
    QPointF position;
    double hspeed{};
    double vspeed{};
    double gravity{};
    double gravityDirection{270.0};
    double imageIndex{};
    double imageSpeed{1.0};
    int depth{};
    bool visible{true};
    bool pendingDestroy{};
    bool mouseOver{};
    std::array<int, 12> alarms;

    void step();
    bool setAlarm(std::size_t index, int steps);
    bool applyAction(const Action &action);
    void executeActions(const ActionsContainer &actions, GameScene &scene,
                        const ActionExecutor &executor, ObjectInstance *other = nullptr);
    void executeEvent(Object::EventType event, GameScene &scene,
                      const ActionExecutor &executor, ObjectInstance *other = nullptr);
    void executeCollisionEvent(const QString &objectName, GameScene &scene,
                               const ActionExecutor &executor, ObjectInstance &other);
};

// Mutable runtime state shared by the OpenGL and Vulkan renderers.
struct GameScene
{
    struct Sprite
    {
        std::vector<std::size_t> frames;
        std::vector<QRect> collisionBounds;
        QPoint origin;
    };

    using Instance = ObjectInstance;

    explicit GameScene(const ProjectContainer &project);

    std::size_t textureFor(const Instance &instance) const;
    QRectF collisionRect(const Instance &instance) const;
    std::vector<const Instance *> drawOrder() const;
    void start(const ActionExecutor &executor);
    void destroyInstance(Instance &instance);
    void queueMouseMove(QPointF position);
    void queueMouseButton(QPointF position, Qt::MouseButton button, bool pressed);
    void queueMouseWheel(QPointF position, int angleDeltaY);
    void queueMouseLeave();
    void resetMouseInput();
    void advanceTo(qint64 elapsedMilliseconds);
    void draw();

    QSize roomSize;
    QString caption;
    int roomSpeed{30};
    std::vector<QImage> textures;
    std::vector<Sprite> sprites;
    std::vector<Instance> instances;

private:
    struct MouseInput
    {
        enum class Kind { Move, Press, Release, Wheel, Leave, Reset } kind;
        QPointF position;
        Qt::MouseButton button{Qt::NoButton};
        int wheelDelta{};
    };

    qint64 m_stepsDone{};
    ActionExecutor m_executor;
    std::vector<MouseInput> m_mouseInputs;
    QPointF m_mousePosition;
    Qt::MouseButtons m_mouseButtons{Qt::NoButton};
    bool m_mouseInside{};
    int m_wheelRemainder{};
    void removeDestroyed();
    void updateMouseHover();
    void dispatchMouseEvent(Object::EventType type, QPointF position, bool global);
    void processMouseInput();
};
