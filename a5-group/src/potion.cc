#include "potion.h"

#include "game.h"
#include "player.h"

Potion::Potion(Vec2D pos, PotionType type)
    : Item(pos)
    , _type{type} {}

PotionType Potion::type() const { return _type; }

void Potion::actBy(Game &g, Player *p) {
    int max_hp, hp;
    switch (_type) {
    case PotionType::RH:
        max_hp = getStat(p->race()).hp;
        hp = p->getHp() + 10;
        p->setHp(max_hp < hp ? max_hp : hp);
        break;
    case PotionType::BA:
        p->addBuff(Buff::atk_buff, 5);
        break;
    case PotionType::BD:
        p->addBuff(Buff::def_buff, 5);
        break;
    case PotionType::PH:
        if (p->race() == Race::elves) {
            max_hp = getStat(p->race()).hp;
            hp = p->getHp() + 10;
            p->setHp(max_hp < hp ? max_hp : hp);
        } else {
            hp = p->getHp() - 10;
        }
        break;
    case PotionType::WA:
        if (p->race() == Race::elves) {
            p->addBuff(Buff::atk_buff, 5);
        } else {
            p->addBuff(Buff::atk_buff, -5);
        }
        break;
    case PotionType::WD:
        if (p->race() == Race::elves) {
            p->addBuff(Buff::def_buff, 5);
        } else {
            p->addBuff(Buff::def_buff, -5);
        }
        break;
    default:
        return;
    }
    p->learnPotion(_type);
    _used = true;
}

std::vector<std::string> Potion::isA() const {
    auto vec = Item::isA();
    vec.emplace_back("Potion");
    switch (_type) {
    case PotionType::RH:
        vec.emplace_back("RH");
        break;
    case PotionType::BA:
        vec.emplace_back("BA");
        break;
    case PotionType::BD:
        vec.emplace_back("BD");
        break;
    case PotionType::PH:
        vec.emplace_back("PH");
        break;
    case PotionType::WA:
        vec.emplace_back("WA");
        break;
    case PotionType::WD:
        vec.emplace_back("WD");
        break;
    }
    return vec;
}
