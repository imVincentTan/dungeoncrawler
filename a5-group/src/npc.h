#ifndef NPC_H
#define NPC_H
#include "character.h"
#include "vec.h"

class NPC : public Character {
  public:
    NPC(Vec2D pos, Race race);
    virtual ~NPC() = default;

    // override Character::move() to attack player
    void move(Game &g) override;

    // override Object::act* to perform actions
    void act(Game &g, Player *p) override;
    void actBy(Game &g, Player *p) override;

    // return ["Character", "[race name]", "NPC"]
    std::vector<std::string> isA() const override;
};

class Dragon final : public NPC {
  public:
    Dragon(Vec2D pos, Vec2D treasure_pos);

    // override NPC::move() to guard treasure
    void move(Game &g) override;

  private:
    Vec2D treasure_pos;
};
#endif
