#ifndef MAJORITEM_H
#define MAJORITEM_H
#include "item.h"

enum class MajorItemType { compass, barrier_suit };

class MajorItem : public Item {
  public:
    MajorItem(Vec2D pos, MajorItemType type);

    // override Object::act* to perform actions
    void actBy(Game &g, Player *p) override;

    // return ["Item", "[type]"]
    std::vector<std::string> isA() const override;

  private:
    MajorItemType _type;
};

class BarrierSuit final : public MajorItem {
  public:
    BarrierSuit(Vec2D pos, Vec2D dragon_pos);

    // override Object::act* to perform actions
    void actBy(Game &g, Player *p) override;

  private:
    Vec2D _dragon_pos;
};
#endif
