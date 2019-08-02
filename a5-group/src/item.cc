#include "item.h"

Item::Item(Vec2D pos)
    : Object(pos)
    , _used{false} {}

bool Item::used() const { return _used; }

std::vector<std::string> Item::isA() const {
    auto vec = Object::isA();
    vec.emplace_back("Item");
    return vec;
}
