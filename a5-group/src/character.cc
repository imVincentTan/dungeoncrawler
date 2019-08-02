#include "character.h"

#include "game.h"

#include <iterator>
#include <utility>

Character::Character(Vec2D pos, CharacterStat s, Race race)
    : Object(pos)
    , _hp{s.hp}
    , _atk{s.atk}
    , _def{s.def}
    , _race{race}
    , _compass{false} {}

void Character::move(Game &g) {
    Level &level = g.level();
    auto space = level.spaceAround(_pos);
    if (!space.empty()) {
        _pos = space[g.randomInt(space.size())].second;
    }
}

int Character::getHp() const { return _hp; }
int Character::getAtk() const { return _atk; }
int Character::getDef() const { return _def; }
void Character::setHp(int hp) { _hp = hp; }
void Character::setAtk(int atk) { _atk = atk; }
void Character::setDef(int def) { _def = def; }
Race Character::race() const { return _race; }
bool &Character::compass() { return _compass; }

std::vector<std::string> Character::isA() const {
    auto vec = Object::isA();
    vec.emplace_back("Character");
    switch (_race) {
    case Race::human:
        vec.emplace_back("Human");
        break;
    case Race::dwarf:
        vec.emplace_back("Dwarf");
        break;
    case Race::elves:
        vec.emplace_back("Elves");
        break;
    case Race::orc:
        vec.emplace_back("Orc");
        break;
    case Race::vampire:
        vec.emplace_back("Vampire");
        break;
    case Race::werewolf:
        vec.emplace_back("Werewolf");
        break;
    case Race::troll:
        vec.emplace_back("Troll");
        break;
    case Race::goblin:
        vec.emplace_back("Goblin");
        break;
    case Race::merchant:
        vec.emplace_back("Merchant");
        break;
    case Race::dragon:
        vec.emplace_back("Dragon");
        break;
    case Race::phoenix:
        vec.emplace_back("Phoenix");
        break;
    }
    return vec;
}
