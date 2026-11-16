#include "projectcontainer.h"

template<> ResourceCollection<Sprite> &ProjectContainer::containerFor() { return sprites; }
template<> const ResourceCollection<Sprite> &ProjectContainer::containerFor() const { return sprites; }
template<> ResourceCollection<Sound> &ProjectContainer::containerFor() { return sounds; }
template<> const ResourceCollection<Sound> &ProjectContainer::containerFor() const { return sounds; }
template<> ResourceCollection<Background> &ProjectContainer::containerFor() { return backgrounds; }
template<> const ResourceCollection<Background> &ProjectContainer::containerFor() const { return backgrounds; }
template<> ResourceCollection<Path> &ProjectContainer::containerFor() { return paths; }
template<> const ResourceCollection<Path> &ProjectContainer::containerFor() const { return paths; }
template<> ResourceCollection<Script> &ProjectContainer::containerFor() { return scripts; }
template<> const ResourceCollection<Script> &ProjectContainer::containerFor() const { return scripts; }
template<> ResourceCollection<Font> &ProjectContainer::containerFor() { return fonts; }
template<> const ResourceCollection<Font> &ProjectContainer::containerFor() const { return fonts; }
template<> ResourceCollection<TimeLine> &ProjectContainer::containerFor() { return timeLines; }
template<> const ResourceCollection<TimeLine> &ProjectContainer::containerFor() const { return timeLines; }
template<> ResourceCollection<Object> &ProjectContainer::containerFor() { return objects; }
template<> const ResourceCollection<Object> &ProjectContainer::containerFor() const { return objects; }
template<> ResourceCollection<Room> &ProjectContainer::containerFor() { return rooms; }
template<> const ResourceCollection<Room> &ProjectContainer::containerFor() const { return rooms; }

bool ProjectContainer::hasResourceName(const QString &name) const
{
    return sprites.find(name) || sounds.find(name) || backgrounds.find(name) ||
           paths.find(name) || scripts.find(name) || fonts.find(name) ||
           timeLines.find(name) || objects.find(name) || rooms.find(name);
}
