#ifndef PLAYER_H
#define PLAYER_H
#include "character.h"

#include <set>
#include <utility>
#include <vector>

enum class PotionType;
enum class Buff { atk_buff, def_buff };

class Player final : public Character {
  public:
    Player(Race race);

    // gold in cent (1/10)
    int getGold() const;
    void addGold(int n);

    bool learnedPotion(PotionType p) const;
    void learnPotion(PotionType p);

    void addBuff(Buff b, int n);
    void clearBuff();

    bool &attackedMerchant();
    bool &barrierSuit();

    // override Character::get{Atk,Def}() to process buffs
    int getAtk() const override;
    int getDef() const override;
    // override Character::move() to process player input
    void move(Game &g) override;

    // override Object::act* to perform actions
    void act(Game &g, Item *i) override;
    void act(Game &g, NPC *n) override;
    void actBy(Game &g, NPC *n) override;

    // return ["Character", "[race name]", "Player"]
    std::vector<std::string> isA() const override;

  private:
    int _gold;
    bool _attacked_merchant;
    bool _barrier_suit;
    std::set<PotionType> _learned_potion;
    std::vector<std::pair<Buff, int>> _buffs;
};
#endif
