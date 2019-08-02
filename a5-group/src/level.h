#ifndef LEVEL_H
#define LEVEL_H
#include "item.h"
#include "npc.h"
#include "vec.h"

#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <vector>

class Game;
class Player;

enum class Tile { empty, v_wall, h_wall, floor, door, passage, stair };

class Level final {
  public:
    Level(Player *player);
    int level() const;
    void setBarrierSuitLevel(int n);

    // load one level from stream
    void loadLevel(std::istream &in, bool enhance);
    // load next level from stream
    void incLevel(std::istream &in, bool enhance);

    // populate level with npc/item/stair
    void populate(Game &g);
    // call move() on all the monsters and sort them after,
    // return false when player died or the level is passed.
    bool nextTurn(Game &g);

    Tile getTile(Vec2D pos) const;
    Item *getItem(Vec2D pos) const;
    NPC *getNPC(Vec2D pos) const;
    std::vector<std::pair<Tile, Vec2D>> spaceAround(Vec2D pos) const;
    std::vector<Item *> itemAround(Vec2D pos) const;

    void addItem(Item *i);
    void addNPC(NPC *n);
    void delItem(Item *i);
    void delNPC(NPC *n);

    int width() const;
    int height() const;
    const std::vector<std::vector<Tile>> &tiles() const;
    const std::vector<std::unique_ptr<NPC>> &npcs() const;
    const std::vector<std::unique_ptr<Item>> &items() const;

  private:
    int _level;
    Player *_player;
    bool _item_from_map_file;
    int _barrier_suit_level;
    std::vector<std::vector<Tile>> _tiles;
    std::vector<std::set<Vec2D>> _chambers;
    std::vector<std::unique_ptr<NPC>> _npcs;
    std::vector<std::unique_ptr<Item>> _items;
    Vec2D _stair_pos;

    // cleanup dead npcs and used items
    void clean(Game &g);
    // remove all the npc/item/stair
    void clear();
    // level loading helper return true if level have npcs/items
    // level generation helper
    Vec2D getSpawnPos(Game &g) const;
    int num_of_chamber() const;
    int num_of_tile(int chamber) const;
    Vec2D nth_tile(int chamber, int n) const;
    int isIn(Vec2D pos) const;
};
#endif
