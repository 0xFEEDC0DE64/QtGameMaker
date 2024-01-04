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
