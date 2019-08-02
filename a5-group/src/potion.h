#ifndef POTION_H
#define POTION_H
#include "item.h"

enum class PotionType { RH = 0, BA = 1, BD = 2, PH = 3, WA = 4, WD = 5 };

class Potion final : public Item {
  public:
    Potion(Vec2D pos, PotionType type);

    PotionType type() const;
    // override Object::act* to perform actions
    void actBy(Game &g, Player *p) override;

    // return ["Item", "Potion", "[type]"]
    std::vector<std::string> isA() const override;

  private:
    PotionType _type;
};
#endif
