#include "npc.h"

#include "game.h"
#include "majoritem.h"
#include "treasure.h"

NPC::NPC(Vec2D pos, Race race)
    : Character(pos, getStat(race), race) {}

void NPC::move(Game &g) {
    Player &p = g.player();
    if (isAdjacent(_pos, p.getPos()) &&
        (_race != Race::merchant || p.attackedMerchant())) {
        g.npc_action().emplace_back(
            Action{Act::attack, dir(p.getPos() - _pos), this, &p});
        act(g, &p);
    } else {
        Character::move(g);
    }
}

void NPC::act(Game &g, Player *p) { p->actBy(g, this); }
void NPC::actBy(Game &g, Player *p) {
    int damage = calcDamage(p->getAtk(), _def, false);
    _hp -= damage;
    g.action().num = damage;
    if (damage && _race == Race::merchant) p->attackedMerchant() = true;
    if (_hp <= 0) {
        switch (_race) {
        case Race::dragon:
            break;
        case Race::merchant:
            g.level().addItem(new Treasure{_pos, GoldPile::merchant_hoard});
            break;
        default:
            p->addGold(10);
        }
        if (_compass)
            g.level().addItem(new MajorItem{_pos, MajorItemType::compass});
    }
}

std::vector<std::string> NPC::isA() const {
    auto vec = Character::isA();
    vec.emplace_back("NPC");
    return vec;
}

Dragon::Dragon(Vec2D pos, Vec2D treasure_pos)
    : NPC(pos, Race::dragon)
    , treasure_pos{treasure_pos} {}

void Dragon::move(Game &g) {
    Player &p = g.player();
    if (isAdjacent(treasure_pos, p.getPos())) {
        g.npc_action().emplace_back(Action{Act::attack, Dir::Null, this, &p});
        act(g, &p);
    }
}
