#include "game.h"

#include "display.h"
#include "item.h"
#include "level.h"
#include "npc.h"
#include "player.h"

#include <random>
#include <vector>

Game::Game(std::vector<Display *> disp, std::string file, unsigned seed,
           bool enhance, bool debug)
    : rng{std::default_random_engine{seed}}
    , disp{disp}
    , _player{disp[0]->selectRace()}
    , _level{&_player}
    , _map{file}
    , _map_file{file}
    , _action{Act::start}
    , _run{true}
    , _enhance{enhance}
    , _debug{debug} {
    _level.loadLevel(_map_file, _enhance);
    _level.setBarrierSuitLevel(randomInt(1, 6));
    _level.populate(*this);
    for (auto d : disp) {
        d->refresh(*this);
    }
}

void Game::start() {
    while (_run) {
        _npc_action.clear();
        disp.front()->getInput(*this);
        if (_action.act == Act::start) {
            init();
        } else if (_action.act == Act::quit) {
            disp[0]->quit();
            break;
        } else if (_level.nextTurn(*this)) {
            for (auto d : disp) {
                d->refresh(*this);
            }
        } else if (_player.getHp() <= 0 || _level.level() == 5) {
            for (auto d : disp) {
                d->printStats(*this);
            }
            if (disp[0]->restart())
                init();
            else
                end();
        } else {
            _level.incLevel(_map_file, _enhance);
            _level.populate(*this);
            for (auto d : disp) {
                d->refresh(*this);
            }
        }
    }
}

void Game::init() {
    _player = Player{disp[0]->selectRace()};
    _level = Level{&_player};
    _map_file = std::fstream{_map};
    _level.loadLevel(_map_file, _enhance);
    _level.setBarrierSuitLevel(randomInt(1, 6));
    _level.populate(*this);
    _npc_action.clear();
    for (auto d : disp) {
        d->refresh(*this);
    }
}

void Game::end() { _run = false; }

int Game::randomInt(int start, int end) {
    std::uniform_int_distribution<int> dist{start, end - 1};
    return dist(rng);
}

int Game::randomInt(int end) {
    std::uniform_int_distribution<int> dist{0, end - 1};
    return dist(rng);
}

bool Game::coinFlip(int p) {
    std::bernoulli_distribution dist{p / 100.0};
    return dist(rng);
}

Action &Game::action() { return _action; }
std::vector<Action> &Game::npc_action() { return _npc_action; }
Level &Game::level() { return _level; }
Player &Game::player() { return _player; }
bool Game::debug() const { return _debug; }
