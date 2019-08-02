#include "treasure.h"

#include "game.h"
#include "player.h"

Treasure::Treasure(Vec2D pos, GoldPile g)
    : Item(pos)
    , _gold{g} {}

void Treasure::actBy(Game &g, Player *p) {
    p->addGold(int(_gold));
    _used = true;
}

DragonHoard::DragonHoard(Vec2D pos, Vec2D dragon_pos)
    : Treasure(pos, GoldPile::dragon_hoard)
    , _dragon_pos{dragon_pos} {}

void DragonHoard::setDragonPos(Vec2D pos) { _dragon_pos = pos; }

void DragonHoard::actBy(Game &g, Player *p) {
    if (!g.level().getNPC(_dragon_pos)) {
        Treasure::actBy(g, p);
    }
}

std::vector<std::string> Treasure::isA() const {
    auto vec = Item::isA();
    vec.emplace_back("Treasure");
    switch (_gold) {
    case GoldPile::normal:
        vec.emplace_back("Normal Hoard");
        break;
    case GoldPile::small_hoard:
        vec.emplace_back("Small Hoard");
        break;
    case GoldPile::merchant_hoard:
        vec.emplace_back("Merchant Hoard");
        break;
    case GoldPile::dragon_hoard:
        vec.emplace_back("Dragon Hoard");
        break;
    }
    return vec;
}
