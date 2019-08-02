#ifndef ITEM_H
#define ITEM_H
#include "object.h"

class Item : public Object {
  public:
    Item(Vec2D pos);
    virtual ~Item() = default;

    bool used() const;

    // return ["Item"]
    std::vector<std::string> isA() const override;

  protected:
    bool _used;
};
#endif
