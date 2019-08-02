#include "majoritem.h"

#include "game.h"
#include "player.h"

MajorItem::MajorItem(Vec2D pos, MajorItemType type)
    : Item(pos)
    , _type{type} {}

void MajorItem::actBy(Game &g, Player *p) {
    if (_type == MajorItemType::compass)
        p->compass() = true;
    else if (_type == MajorItemType::barrier_suit)
        p->barrierSuit() = true;
    _used = true;
}

BarrierSuit::BarrierSuit(Vec2D pos, Vec2D dragon_pos)
    : MajorItem(pos, MajorItemType::barrier_suit)
    , _dragon_pos{dragon_pos} {}

void BarrierSuit::actBy(Game &g, Player *p) {
    if (!g.level().getNPC(_dragon_pos)) {
        MajorItem::actBy(g, p);
    }
}

std::vector<std::string> MajorItem::isA() const {
    auto vec = Item::isA();
    vec.emplace_back("Major Item");
    switch (_type) {
    case MajorItemType::compass:
        vec.emplace_back("Compass");
        break;
    case MajorItemType::barrier_suit:
        vec.emplace_back("Barrier Suit");
        break;
    }
    return vec;
}
