#include "player.h"

#include "game.h"

Player::Player(Race race)
    : Character({}, getStat(race), race)
    , _gold{0}
    , _attacked_merchant{false}
    , _barrier_suit{false} {}

int Player::getGold() const {
    if (_race == Race::dwarf)
        return _gold * 2;
    else if (_race == Race::orc)
        return _gold / 2;
    else
        return _gold;
}
void Player::addGold(int n) { _gold += n; }

bool Player::learnedPotion(PotionType p) const {
    return _learned_potion.find(p) != _learned_potion.end();
}
void Player::learnPotion(PotionType p) { _learned_potion.emplace(p); }
void Player::addBuff(Buff b, int n) {
    _buffs.emplace_back(std::pair<Buff, int>{b, n});
}
void Player::clearBuff() { _buffs.clear(); }

bool &Player::attackedMerchant() { return _attacked_merchant; }
bool &Player::barrierSuit() { return _barrier_suit; }

int Player::getAtk() const {
    int atk = _atk;
    for (auto &buff : _buffs) {
        if (buff.first == Buff::atk_buff) atk += buff.second;
    }
    if (atk < 0) atk = 0;
    return atk;
}

int Player::getDef() const {
    int def = _def;
    for (auto &buff : _buffs) {
        if (buff.first == Buff::def_buff) def += buff.second;
    }
    if (def < 0) def = 0;
    return def;
}

void Player::move(Game &g) { // TODO
    Action &action = g.action();
    action.actor = this;
    Vec2D pos = _pos + action.dir;
    NPC *npc = g.level().getNPC(pos);
    Item *item = g.level().getItem(pos);
    Tile t = g.level().getTile(pos);
    switch (action.act) {
    case Act::move:
        if (item && item->isA()[1] == "Treasure") {
            act(g, item);
            if (item->used()) item = nullptr;
        }
        if (!npc && !item &&
            (t == Tile::floor || t == Tile::door || t == Tile::passage ||
             t == Tile::stair))
            _pos = pos;
        else
            action.dir = Dir::Null;
        break;
    case Act::attack:
        if (npc) {
            act(g, npc);
            action.actee = npc;
            action.item = nullptr;
        }
        break;
    case Act::use:
        if (item) {
            act(g, item);
            action.actee = nullptr;
            action.item = item;
        }
        break;
    default:
        break;
    }
    action.actee = npc;
    action.item = item;
}

void Player::act(Game &g, Item *i) { i->actBy(g, this); }
void Player::act(Game &g, NPC *n) { n->actBy(g, this); }
void Player::actBy(Game &g, NPC *n) {
    if (g.coinFlip()) {
        int damage = calcDamage(n->getAtk(), _def, _barrier_suit);
        _hp -= damage;
        g.npc_action().back().num = damage;
    } else {
        g.npc_action().back().num = 0;
    }
}

std::vector<std::string> Player::isA() const {
    auto vec = Character::isA();
    vec.emplace_back("Player");
    return vec;
}
