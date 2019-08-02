#ifndef TREASURE_H
#define TREASURE_H
#include "item.h"
#include "vec.h"

enum class GoldPile {
    normal = 10,
    small_hoard = 20,
    merchant_hoard = 40,
    dragon_hoard = 60
};

class Treasure : public Item {
  public:
    Treasure(Vec2D pos, GoldPile g);

    // override Object::act* to perform actions
    void actBy(Game &g, Player *p) override;

    // return ["Item", "Treasure", "[type]"]
    std::vector<std::string> isA() const override;

  protected:
    GoldPile _gold;
};

class DragonHoard : public Treasure {
  public:
    DragonHoard(Vec2D pos, Vec2D dragon_pos);

    void setDragonPos(Vec2D pos);
    // override Object::act* to perform actions
    void actBy(Game &g, Player *p) override;

  private:
    Vec2D _dragon_pos;
};
#endif
