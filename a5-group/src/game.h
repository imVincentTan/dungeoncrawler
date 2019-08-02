#ifndef GAME_H
#define GAME_H
#include "level.h"
#include "player.h"

#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <vector>

class Display;

enum class Act { move, use, attack, start, quit, race };
enum class Dir;

struct Action {
    Act act;
    Dir dir;
    Character *actor;
    Character *actee;
    Item *item;
    int num;
};

class Game final {
  public:
    Game(std::vector<Display *> disp, std::string file, unsigned seed = 0,
         bool enhance = false, bool debug = false);
    Action &action();
    std::vector<Action> &npc_action();
    Level &level();
    Player &player();
    bool debug() const;

    void start();
    void end();

    // generate a int in the range [start, end)
    int randomInt(int start, int end);
    int randomInt(int end);
    // has a p% probability of being true
    bool coinFlip(int p = 50);

  private:
    std::default_random_engine rng;
    std::vector<Display *> disp;
    Player _player;
    Level _level;
    std::string _map;
    std::fstream _map_file;
    Action _action;
    std::vector<Action> _npc_action;
    bool _run;
    bool _enhance;
    bool _debug;

    // restart game
    void init();
};
#endif
