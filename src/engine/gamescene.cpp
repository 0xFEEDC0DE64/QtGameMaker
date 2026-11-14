#include "gamescene.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <type_traits>
#include <utility>

#include <QHash>
#include <QRandomGenerator>

namespace {

std::size_t frameIndex(double imageIndex, std::size_t count)
{
    const auto size = static_cast<qint64>(count);
    qint64 index = static_cast<qint64>(std::floor(imageIndex)) % size;
    if (index < 0)
        index += size;
    return static_cast<std::size_t>(index);
}

QPointF motion(double degrees, double speed)
{
    const double radians = degrees * std::numbers::pi / 180.0;
    return {speed * std::cos(radians), -speed * std::sin(radians)};
}

std::optional<Object::EventType> mouseButtonEvent(Qt::MouseButton button, bool pressed, bool global)
{
    using Event = Object::EventType;
    switch (button)
    {
    case Qt::LeftButton:
        return global ? (pressed ? Event::MouseGlobalLeftPressed : Event::MouseGlobalLeftReleased)
                      : (pressed ? Event::MouseLeftPressed : Event::MouseLeftReleased);
    case Qt::RightButton:
        return global ? (pressed ? Event::MouseGlobalRightPressed : Event::MouseGlobalRightReleased)
                      : (pressed ? Event::MouseRightPressed : Event::MouseRightReleased);
    case Qt::MiddleButton:
        return global ? (pressed ? Event::MouseGlobalMiddlePressed : Event::MouseGlobalMiddleReleased)
                      : (pressed ? Event::MouseMiddlePressed : Event::MouseMiddleReleased);
    default:
        return std::nullopt;
    }
}

std::optional<Object::EventType> mouseHeldEvent(Qt::MouseButton button, bool global)
{
    using Event = Object::EventType;
    switch (button)
    {
    case Qt::LeftButton: return global ? Event::MouseGlobalLeftButton : Event::MouseLeftButton;
    case Qt::RightButton: return global ? Event::MouseGlobalRightButton : Event::MouseRightButton;
    case Qt::MiddleButton: return global ? Event::MouseGlobalMiddleButton : Event::MouseMiddleButton;
    default: return std::nullopt;
    }
}

} // namespace

void ObjectInstance::executeActions(const ActionsContainer &actions, GameScene &scene,
                                    const ActionExecutor &executor, ObjectInstance *other)
{
    if (!executor)
        return;

    for (const Action &action : actions)
    {
        const AppliesTo &appliesTo = std::visit([](const auto &value) -> const AppliesTo & {
            return value.appliesTo;
        }, action);
        std::visit([&](const auto &target) {
            using Target = std::decay_t<decltype(target)>;
            if constexpr (std::is_same_v<Target, AppliesToSelf>)
                executor(*this, action, other);
            else if constexpr (std::is_same_v<Target, AppliesToOther>)
            {
                if (other)
                    executor(*other, action, this);
            }
            else
            {
                for (ObjectInstance &instance : scene.instances)
                {
                    if (instance.definition && instance.definition->name == target.objectName)
                        executor(instance, action, this);
                }
            }
        }, appliesTo);
    }
}

void ObjectInstance::executeEvent(Object::EventType event, GameScene &scene,
                                  const ActionExecutor &executor, ObjectInstance *other)
{
    if (!definition)
        return;
    const auto found = definition->events.find(event);
    if (found != definition->events.end())
        executeActions(found->second, scene, executor, other);
}

void ObjectInstance::executeCollisionEvent(const QString &objectName, GameScene &scene,
                                           const ActionExecutor &executor, ObjectInstance &other)
{
    if (!definition)
        return;
    const auto found = definition->collisionEvents.find(objectName);
    if (found != definition->collisionEvents.end())
        executeActions(found->second, scene, executor, &other);
}

void ObjectInstance::step()
{
    const QPointF gravityMotion = motion(gravityDirection, gravity);
    hspeed += gravityMotion.x();
    vspeed += gravityMotion.y();
    position += QPointF{hspeed, vspeed};
    imageIndex += imageSpeed;
}

bool ObjectInstance::setAlarm(std::size_t index, int steps)
{
    if (index >= alarms.size())
        return false;
    alarms[index] = steps > 0 ? steps : -1;
    return true;
}

bool ObjectInstance::applyAction(const Action &action)
{
    return std::visit([this](const auto &value) -> bool {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, SpeedHorizontalAction>)
            hspeed = value.relative ? hspeed + value.speed : value.speed;
        else if constexpr (std::is_same_v<T, SpeedVerticalAction>)
            vspeed = value.relative ? vspeed + value.speed : value.speed;
        else if constexpr (std::is_same_v<T, SetGravityAction>)
        {
            gravityDirection = value.relative ? gravityDirection + value.direction : value.direction;
            gravity = value.relative ? gravity + value.gravity : value.gravity;
        }
        else if constexpr (std::is_same_v<T, MoveFreeAction>)
        {
            const QPointF velocity = motion(value.direction, value.speed);
            hspeed = value.relative ? hspeed + velocity.x() : velocity.x();
            vspeed = value.relative ? vspeed + velocity.y() : velocity.y();
        }
        else if constexpr (std::is_same_v<T, MoveTowardsAction>)
        {
            const QPointF offset{double(value.x), double(value.y)};
            const QPointF goal = value.relative ? position + offset : offset;
            const QPointF delta = goal - position;
            const double distance = std::hypot(delta.x(), delta.y());
            if (distance > 0)
            {
                hspeed = value.speed * delta.x() / distance;
                vspeed = value.speed * delta.y() / distance;
            }
            else
                hspeed = vspeed = 0;
        }
        else if constexpr (std::is_same_v<T, MoveFixedAction>)
        {
            std::array<int, 9> choices{};
            int count = 0;
            for (int i = 0; i < int(choices.size()); ++i)
                if (value.directions[std::size_t(i)])
                    choices[std::size_t(count++)] = i;
            if (count == 0)
                return true;
            const int direction = choices[QRandomGenerator::global()->bounded(count)];
            constexpr std::array<double, 9> angles{135, 90, 45, 180, 0, 0, 225, 270, 315};
            const QPointF velocity = direction == 4 ? QPointF{} : motion(angles[std::size_t(direction)], value.speed);
            hspeed = value.relative ? hspeed + velocity.x() : velocity.x();
            vspeed = value.relative ? vspeed + velocity.y() : velocity.y();
        }
        else
            return false; // ExecuteCode needs a GML interpreter.
        return true;
    }, action);
}

GameScene::GameScene(const ProjectContainer &project)
{
    if (project.rooms.empty())
        return;

    const Room &room = project.rooms.front();
    roomSize = QSize{std::max(1, room.width), std::max(1, room.height)};
    caption = room.caption.isEmpty() ? room.name : room.caption;
    roomSpeed = std::max(1, room.speed);

    QHash<QString, std::size_t> spriteIndices;
    for (const ::Sprite &source : project.sprites)
    {
        Sprite sprite;
        sprite.origin = QPoint{source.origin.x, source.origin.y};
        for (const QPixmap &pixmap : source.pixmaps)
        {
            if (pixmap.isNull())
                continue;
            sprite.frames.push_back(textures.size());
            textures.push_back(pixmap.toImage());
            sprite.collisionBounds.push_back(QRect{QPoint{}, textures.back().size()});
        }
        spriteIndices.insert(source.name, sprites.size());
        sprites.push_back(std::move(sprite));
    }

    QHash<QString, const Object *> objects;
    for (const Object &object : project.objects)
        objects.insert(object.name, &object);

    for (const Room::Object &placed : room.objects)
    {
        const Object *object = objects.value(placed.objectName, nullptr);
        if (!object)
            continue;
        const auto sprite = spriteIndices.constFind(object->spriteName);
        Instance instance;
        instance.definition = object;
        if (sprite != spriteIndices.cend() && !sprites.at(*sprite).frames.empty())
            instance.spriteIndex = *sprite;
        const auto mask = spriteIndices.constFind(object->maskSpriteName);
        if (!object->maskSpriteName.isEmpty() && mask != spriteIndices.cend() &&
            !sprites.at(*mask).frames.empty())
            instance.maskSpriteIndex = *mask;
        instance.position = QPointF{placed.pos};
        instance.depth = object->depth;
        instance.visible = object->visible;
        instances.push_back(instance);
    }

}

std::size_t GameScene::textureFor(const Instance &instance) const
{
    const Sprite &sprite = sprites.at(*instance.spriteIndex);
    return sprite.frames.at(frameIndex(instance.imageIndex, sprite.frames.size()));
}

QRectF GameScene::collisionRect(const Instance &instance) const
{
    const auto index = instance.maskSpriteIndex ? instance.maskSpriteIndex : instance.spriteIndex;
    if (!index)
        return {};
    const Sprite &sprite = sprites.at(*index);
    const QRect &bounds = sprite.collisionBounds.at(frameIndex(instance.imageIndex, sprite.frames.size()));
    if (bounds.isEmpty())
        return {};
    return {instance.position.x() - sprite.origin.x() + bounds.x(),
            instance.position.y() - sprite.origin.y() + bounds.y(),
            double(bounds.width()), double(bounds.height())};
}

std::vector<const GameScene::Instance *> GameScene::drawOrder() const
{
    std::vector<const Instance *> order;
    order.reserve(instances.size());
    for (const Instance &instance : instances)
        order.push_back(&instance);
    // Larger depths are behind smaller ones; equal depths keep room placement order.
    std::stable_sort(order.begin(), order.end(), [](const Instance *left, const Instance *right) {
        return left->depth > right->depth;
    });
    return order;
}

void GameScene::start(const ActionExecutor &executor)
{
    m_executor = executor;
    m_stepsDone = 0;
    for (Instance &instance : instances)
        if (!instance.pendingDestroy)
            instance.executeEvent(Object::EventType::Create, *this, m_executor);
    removeDestroyed();
}

void GameScene::destroyInstance(Instance &instance)
{
    if (instance.pendingDestroy)
        return;
    instance.pendingDestroy = true;
    instance.executeEvent(Object::EventType::Destroy, *this, m_executor);
}

void GameScene::removeDestroyed()
{
    std::erase_if(instances, [](const Instance &instance) { return instance.pendingDestroy; });
}

void GameScene::queueMouseMove(QPointF position)
{
    m_mouseInputs.push_back({MouseInput::Kind::Move, position});
}

void GameScene::queueMouseButton(QPointF position, Qt::MouseButton button, bool pressed)
{
    m_mouseInputs.push_back({pressed ? MouseInput::Kind::Press : MouseInput::Kind::Release,
                             position, button});
}

void GameScene::queueMouseWheel(QPointF position, int angleDeltaY)
{
    m_mouseInputs.push_back({MouseInput::Kind::Wheel, position, Qt::NoButton, angleDeltaY});
}

void GameScene::queueMouseLeave()
{
    m_mouseInputs.push_back({MouseInput::Kind::Leave, {}});
}

void GameScene::resetMouseInput()
{
    m_mouseInputs.push_back({MouseInput::Kind::Reset, {}});
}

void GameScene::dispatchMouseEvent(Object::EventType type, QPointF position, bool global)
{
    for (Instance &instance : instances)
        if (!instance.pendingDestroy &&
            (global || collisionRect(instance).contains(position)))
            instance.executeEvent(type, *this, m_executor);
}

void GameScene::updateMouseHover()
{
    for (Instance &instance : instances)
    {
        if (instance.pendingDestroy)
            continue;
        const bool over = m_mouseInside && collisionRect(instance).contains(m_mousePosition);
        if (over == instance.mouseOver)
            continue;
        instance.mouseOver = over;
        instance.executeEvent(over ? Object::EventType::MouseEnter : Object::EventType::MouseLeave,
                              *this, m_executor);
    }
}

void GameScene::processMouseInput()
{
    const auto inputs = std::exchange(m_mouseInputs, {});
    const QRectF room{QPointF{}, QSizeF{roomSize}};
    for (const MouseInput &input : inputs)
    {
        if (input.kind == MouseInput::Kind::Leave || input.kind == MouseInput::Kind::Reset)
        {
            m_mouseInside = false;
            if (input.kind == MouseInput::Kind::Reset)
                m_mouseButtons = Qt::NoButton;
            updateMouseHover();
            continue;
        }

        m_mousePosition = input.position;
        m_mouseInside = room.contains(input.position);
        updateMouseHover();
        if (input.kind == MouseInput::Kind::Press || input.kind == MouseInput::Kind::Release)
        {
            const bool pressed = input.kind == MouseInput::Kind::Press;
            if (pressed)
                m_mouseButtons |= input.button;
            else
                m_mouseButtons &= ~Qt::MouseButtons{input.button};
            if (const auto local = mouseButtonEvent(input.button, pressed, false); local && m_mouseInside)
                dispatchMouseEvent(*local, input.position, false);
            if (const auto global = mouseButtonEvent(input.button, pressed, true); global && m_mouseInside)
                dispatchMouseEvent(*global, input.position, true);
        }
        else if (input.kind == MouseInput::Kind::Wheel && m_mouseInside)
        {
            m_wheelRemainder += input.wheelDelta;
            while (m_wheelRemainder >= 120)
            {
                dispatchMouseEvent(Object::EventType::MouseWheelUp, input.position, true);
                m_wheelRemainder -= 120;
            }
            while (m_wheelRemainder <= -120)
            {
                dispatchMouseEvent(Object::EventType::MouseWheelDown, input.position, true);
                m_wheelRemainder += 120;
            }
        }
    }

    updateMouseHover(); // Instances can also move under a stationary cursor.
    if (!m_mouseInside)
        return;
    if (m_mouseButtons == Qt::NoButton)
        dispatchMouseEvent(Object::EventType::MouseNoButton, m_mousePosition, false);
    for (Qt::MouseButton button : {Qt::LeftButton, Qt::RightButton, Qt::MiddleButton})
    {
        if (!(m_mouseButtons & button))
            continue;
        dispatchMouseEvent(*mouseHeldEvent(button, false), m_mousePosition, false);
        dispatchMouseEvent(*mouseHeldEvent(button, true), m_mousePosition, true);
    }
}

void GameScene::advanceTo(qint64 elapsedMilliseconds)
{
    const qint64 targetStep = elapsedMilliseconds * roomSpeed / 1000;
    while (m_stepsDone < targetStep)
    {
        for (Instance &instance : instances)
            if (!instance.pendingDestroy)
                instance.executeEvent(Object::EventType::BeginStep, *this, m_executor);
        for (Instance &instance : instances)
        {
            if (instance.pendingDestroy)
                continue;
            for (std::size_t index = 0; index < instance.alarms.size(); ++index)
            {
                int &remaining = instance.alarms[index];
                if (remaining <= 0 || --remaining != 0)
                    continue;
                remaining = -1;
                const auto event = static_cast<Object::EventType>(int(Object::EventType::Alarm0) + int(index));
                instance.executeEvent(event, *this, m_executor);
                if (instance.pendingDestroy)
                    break;
            }
        }
        processMouseInput();
        for (Instance &instance : instances)
            if (!instance.pendingDestroy)
                instance.executeEvent(Object::EventType::Step, *this, m_executor);

        std::vector<QPointF> previousPositions;
        previousPositions.reserve(instances.size());
        for (const Instance &instance : instances)
            previousPositions.push_back(instance.position);
        for (Instance &instance : instances)
            if (!instance.pendingDestroy)
                instance.step();
        updateMouseHover();

        // Detect every pair at its moved position before collision actions can
        // change either instance. A sprite-size rectangle is our first mask.
        std::vector<std::pair<std::size_t, std::size_t>> collisions;
        for (std::size_t i = 0; i < instances.size(); ++i)
        {
            if (instances[i].pendingDestroy)
                continue;
            const QRectF first = collisionRect(instances[i]);
            if (first.isEmpty())
                continue;
            for (std::size_t j = i + 1; j < instances.size(); ++j)
            {
                if (instances[j].pendingDestroy)
                    continue;
                const QRectF second = collisionRect(instances[j]);
                if (!second.isEmpty() && first.intersects(second))
                    collisions.emplace_back(i, j);
            }
        }
        for (const auto &[i, j] : collisions)
        {
            Instance &first = instances[i];
            Instance &second = instances[j];
            if (first.pendingDestroy || second.pendingDestroy)
                continue;
            const QString &firstName = first.definition->name;
            const QString &secondName = second.definition->name;
            const auto firstEvent = first.definition->collisionEvents.find(secondName);
            const auto secondEvent = second.definition->collisionEvents.find(firstName);
            const bool firstHandles = firstEvent != first.definition->collisionEvents.end() &&
                                      !firstEvent->second.empty();
            const bool secondHandles = secondEvent != second.definition->collisionEvents.end() &&
                                       !secondEvent->second.empty();
            if (!firstHandles && !secondHandles)
                continue;

            if (first.definition->solid || second.definition->solid)
            {
                first.position = previousPositions[i];
                second.position = previousPositions[j];
            }
            if (firstHandles)
                first.executeCollisionEvent(secondName, *this, m_executor, second);
            if (secondHandles && !second.pendingDestroy && !first.pendingDestroy)
                second.executeCollisionEvent(firstName, *this, m_executor, first);
        }
        for (Instance &instance : instances)
            if (!instance.pendingDestroy)
                instance.executeEvent(Object::EventType::EndStep, *this, m_executor);
        removeDestroyed();
        ++m_stepsDone;
    }
}

void GameScene::draw()
{
    for (Instance &instance : instances)
        if (instance.visible && !instance.pendingDestroy)
            instance.executeEvent(Object::EventType::Draw, *this, m_executor);
    removeDestroyed();
}
