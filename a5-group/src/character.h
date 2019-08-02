#ifndef CHARACTER_H
#define CHARACTER_H
#include "object.h"

#include <cmath>

enum class Race;
struct CharacterStat;

class Character : public Object {
  public:
    Character(Vec2D pos, CharacterStat s, Race race);
    virtual ~Character() = default;

    // Character::move() performs random move to any avaliable floor tile
    virtual void move(Game &g);

    int getHp() const;
    virtual int getAtk() const;
    virtual int getDef() const;
    void setHp(int hp);
    void setAtk(int atk);
    void setDef(int def);
    Race race() const;
    bool &compass();

    // return ["Character", "[race name]"]
    std::vector<std::string> isA() const override;

  protected:
    int _hp;
    int _atk;
    int _def;
    Race _race;
    bool _compass;
};

enum class Race {
    human,
    dwarf,
    elves,
    orc,
    vampire,
    werewolf,
    troll,
    goblin,
    merchant,
    dragon,
    phoenix
};

struct CharacterStat {
    int hp;
    int atk;
    int def;
};

static inline CharacterStat getStat(Race race) {
    switch (race) {
    case Race::human:
        return {140, 20, 20};
    case Race::dwarf:
        return {100, 20, 30};
    case Race::elves:
        return {140, 30, 10};
    case Race::orc:
        return {180, 30, 25};
    case Race::vampire:
        return {50, 25, 25};
    case Race::werewolf:
        return {120, 30, 5};
    case Race::troll:
        return {120, 25, 15};
    case Race::goblin:
        return {70, 5, 10};
    case Race::merchant:
        return {30, 70, 5};
    case Race::dragon:
        return {150, 20, 20};
    case Race::phoenix:
        return {50, 35, 20};
    default:
        throw "Unknown Race";
    }
}

static inline int calcDamage(int atk, int def, bool barrier_suit) {
    double damage = 100.0 * atk / (100 + def);
    return ceil(barrier_suit ? damage / 2 : damage);
}
#endif
