#ifndef OBJECT_H
#define OBJECT_H
#include "vec.h"

#include <string>
#include <vector>

class Game;
class Item;
class NPC;
class Player;

class Object {
  public:
    Object(Vec2D pos);
    virtual ~Object() = default;

    // Object does nothing by default
    virtual void act(Game &g, Item *i){};
    virtual void act(Game &g, NPC *n){};
    virtual void act(Game &g, Player *p){};
    virtual void actBy(Game &g, NPC *n){};
    virtual void actBy(Game &g, Player *p){};

    Vec2D getPos() const;
    void setPos(Vec2D pos);

    // ordering according to pos, y first, then x
    bool operator<(const Object &o) const;

    virtual std::vector<std::string> isA() const;

  protected:
    Vec2D _pos;
};
#endif
