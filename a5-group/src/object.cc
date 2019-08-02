#include "object.h"

Object::Object(Vec2D pos)
    : _pos{pos} {}

Vec2D Object::getPos() const { return _pos; }
void Object::setPos(Vec2D pos) { _pos = pos; }

bool Object::operator<(const Object &o) const {
    Vec2D pos = o.getPos();
    return _pos.y < pos.y || (_pos.y == pos.y && _pos.x < pos.x);
}

std::vector<std::string> Object::isA() const {
    std::vector<std::string> vec;
    vec.reserve(3);
    return vec;
}
