#include "level.h"

#include "game.h"
#include "majoritem.h"
#include "npc.h"
#include "player.h"
#include "potion.h"
#include "randMapGen.h"
#include "treasure.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

static const char *MARKER =
    R"(|-----------------------------------------------------------------------------|)";
static const char *DEFAULT_MAP =
    R"(|-----------------------------------------------------------------------------|
|                                                                             |
| |--------------------------|        |-----------------------|               |
| |..........................|        |.......................|               |
| |..........................+########+.......................|-------|       |
| |..........................|   #    |...............................|--|    |
| |..........................|   #    |..................................|--| |
| |----------+---------------|   #    |----+----------------|...............| |
|            #                 #############                |...............| |
|            #                 #     |-----+------|         |...............| |
|            #                 #     |............|         |...............| |
|            ###################     |............|   ######+...............| |
|            #                 #     |............|   #     |...............| |
|            #                 #     |-----+------|   #     |--------+------| |
|  |---------+-----------|     #           #          #              #        |
|  |.....................|     #           #          #         |----+------| |
|  |.....................|     ########################         |...........| |
|  |.....................|     #           #                    |...........| |
|  |.....................|     #    |------+--------------------|...........| |
|  |.....................|     #    |.......................................| |
|  |.....................+##########+.......................................| |
|  |.....................|          |.......................................| |
|  |---------------------|          |---------------------------------------| |
|                                                                             |
|-----------------------------------------------------------------------------|)";

Level::Level(Player *player)
    : _level{1}
    , _player{player} {}

int Level::level() const { return _level; }
void Level::incLevel(std::istream &in, bool enhance) {
    loadLevel(in, enhance);
    ++_level;
}

static inline void canReach(std::vector<std::vector<Tile>> &map,
                            std::set<Vec2D> &reached, Vec2D pos, Vec2D dir);

void Level::loadLevel(std::istream &in, bool enhance) {
    // load tile map
    std::istringstream iss;
    if (enhance) {
        iss = std::istringstream{randMap()};
    } else {
        iss = std::istringstream{DEFAULT_MAP};
    }
    std::istream &real_in = in.peek() ? in : iss;
    clear();
    _item_from_map_file = false;
    int marker = 2;
    int x = 0;
    int y = 0;
    char c;
    std::string sline;
    while (marker && std::getline(real_in, sline)) {
        if (sline == MARKER) --marker;
        x = 0;
        _tiles.emplace_back(std::vector<Tile>{});
        auto &line = _tiles.back();
        std::istringstream iss{sline};
        line.reserve(sline.length() - 1);
        iss >> std::noskipws;
        while (iss >> c) {
            switch (c) {
            case ' ':
                line.emplace_back(Tile::empty);
                break;
            case '|':
                line.emplace_back(Tile::v_wall);
                break;
            case '-':
                line.emplace_back(Tile::h_wall);
                break;
            case '.':
                line.emplace_back(Tile::floor);
                break;
            case '+':
                line.emplace_back(Tile::door);
                break;
            case '#':
                line.emplace_back(Tile::passage);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                _item_from_map_file = true;
                line.emplace_back(Tile::floor);
                _items.emplace_back(new Potion{{x, y}, PotionType(c - '0')});
                break;
            case '6':
                _item_from_map_file = true;
                line.emplace_back(Tile::floor);
                _items.emplace_back(new Treasure{{x, y}, GoldPile::normal});
                break;
            case '7':
                _item_from_map_file = true;
                line.emplace_back(Tile::floor);
                _items.emplace_back(
                    new Treasure{{x, y}, GoldPile::small_hoard});
                break;
            case '8':
                _item_from_map_file = true;
                line.emplace_back(Tile::floor);
                _items.emplace_back(
                    new Treasure{{x, y}, GoldPile::merchant_hoard});
                break;
            case '9':
                _item_from_map_file = true;
                line.emplace_back(Tile::floor);
                _items.emplace_back(new DragonHoard{{x, y}, {0, 0}});
                break;
            }
            ++x;
        }
        ++y;
    }
    _tiles.shrink_to_fit();

    // calculate chamber
    bool new_chamber = true;
    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            if (_tiles[y][x] == Tile::floor) {
                for (auto &chamber : _chambers) {
                    new_chamber &= chamber.find({x, y}) == chamber.end();
                }
                if (new_chamber) {
                    _chambers.emplace_back();
                    canReach(_tiles, _chambers.back(), {x, y}, {});
                }
            }
            new_chamber = true;
        }
    }
}

void Level::setBarrierSuitLevel(int n) { _barrier_suit_level = n; }

void Level::populate(Game &g) {
    int rand;
    Vec2D pos;

    g.action().act = Act::start;

    // set player position
    _player->setPos(getSpawnPos(g));
    int player_chamber = isIn(_player->getPos());

    // generate staircase
    _stair_pos = getSpawnPos(g);
    while (isIn(_stair_pos) == player_chamber) {
        _stair_pos = getSpawnPos(g);
    }
    _tiles[_stair_pos.y][_stair_pos.x] = Tile::stair;

    // generate barrier suit
    if (_level == _barrier_suit_level) {
        pos = getSpawnPos(g);
        auto space = spaceAround(pos);
        auto dragon_pos = space[g.randomInt(space.size())].second;
        _items.emplace_back(new BarrierSuit{pos, dragon_pos});
        _npcs.emplace_back(new Dragon{dragon_pos, pos});
    }

    if (_item_from_map_file) {
        for (auto &item : _items) {
            if (item->isA()[2] == "Dragon Hoard") {
                auto hoard = (DragonHoard *)item.get();
                pos = hoard->getPos();
                auto space = spaceAround(pos);
                auto dragon_pos = space[g.randomInt(space.size())].second;
                hoard->setDragonPos(dragon_pos);
                _npcs.emplace_back(new Dragon{dragon_pos, pos});
            }
        }
    } else {
        // generate potions
        for (int i = 0; i < 10; i++) {
            pos = getSpawnPos(g);
            rand = g.randomInt(6);
            _items.emplace_back(new Potion{pos, PotionType(rand)});
        }

        // generate treasure
        for (int i = 0; i < 10; i++) {
            pos = getSpawnPos(g);
            rand = g.randomInt(8);
            if (rand == 0) {
                auto space = spaceAround(pos);
                int selectDragon = g.randomInt(space.size());
                _items.emplace_back(
                    new DragonHoard{pos, space[selectDragon].second});
                _npcs.emplace_back(new Dragon{space[selectDragon].second, pos});
            } else if (rand < 6) {
                _items.emplace_back(new Treasure{pos, GoldPile::normal});
            } else {
                _items.emplace_back(new Treasure{pos, GoldPile::small_hoard});
            }
        }
    }

    // generating npcs
    while (int(_npcs.size()) < 20) {
        pos = getSpawnPos(g);
        rand = g.randomInt(18);
        Race race;
        if (rand < 4) {
            race = Race::werewolf;
        } else if (rand < 7) {
            race = Race::vampire;
        } else if (rand < 12) {
            race = Race::goblin;
        } else if (rand < 14) {
            race = Race::troll;
        } else if (rand < 16) {
            race = Race::phoenix;
        } else {
            race = Race::merchant;
        }
        _npcs.emplace_back(new NPC{pos, race});
    }
    rand = g.randomInt(20);
    while (_npcs[rand]->isA()[2] == "Merchant") rand = g.randomInt(20);
    _npcs[rand]->compass() = true;
}

bool Level::nextTurn(Game &g) {
    clean(g);
    _player->move(g);
    if (_player->getPos() == _stair_pos) return false;
    for (auto &npc : _npcs) {
        if (npc->getHp() > 0) npc->move(g);
    }
    if (_player->getHp() <= 0) return false;
    sort(_npcs.begin(), _npcs.end());
    return true;
}

Tile Level::getTile(Vec2D pos) const { return _tiles[pos.y][pos.x]; }

Item *Level::getItem(Vec2D pos) const {
    for (auto &item : _items) {
        if (item->getPos() == pos) return item.get();
    }
    return nullptr;
}

NPC *Level::getNPC(Vec2D pos) const {
    for (auto &npc : _npcs) {
        if (npc->getPos() == pos) return npc.get();
    }
    return nullptr;
}

std::vector<std::pair<Tile, Vec2D>> Level::spaceAround(Vec2D pos) const {
    std::vector<std::pair<Tile, Vec2D>> vec;
    vec.reserve(8);
    for (int i = int(Dir::UL); i <= int(Dir::DR); ++i) {
        Vec2D next = pos + Dir(i);
        if (getTile(next) == Tile::floor && next != _player->getPos() &&
            !(getItem(next) || getNPC(next)))
            vec.emplace_back(getTile(next), next);
    }
    return vec;
}

std::vector<Item *> Level::itemAround(Vec2D pos) const {
    std::vector<Item *> vec;
    vec.reserve(8);
    for (int i = int(Dir::UL); i <= int(Dir::DR); ++i) {
        Vec2D next = pos + Dir(i);
        Item *item = getItem(next);
        if (item) vec.emplace_back(item);
    }
    return vec;
}

void Level::addItem(Item *i) {
    _items.emplace_back(i);
    sort(_items.begin(), _items.end());
}

void Level::addNPC(NPC *n) {
    _npcs.emplace_back(n);
    sort(_npcs.begin(), _npcs.end());
}

void Level::delItem(Item *i) {
    for (auto item = _items.begin(); item != _items.end(); ++item) {
        if (item->get() == i) {
            _items.erase(item);
            break;
        }
    }
}

void Level::delNPC(NPC *n) {
    for (auto npc = _npcs.begin(); npc != _npcs.end(); ++npc) {
        if (npc->get() == n) {
            _npcs.erase(npc);
            break;
        }
    }
}

int Level::width() const { return int(_tiles[0].size()); }
int Level::height() const { return int(_tiles.size()); }
const std::vector<std::vector<Tile>> &Level::tiles() const { return _tiles; }
const std::vector<std::unique_ptr<NPC>> &Level::npcs() const { return _npcs; }
const std::vector<std::unique_ptr<Item>> &Level::items() const {
    return _items;
}

void Level::clean(Game &g) {

    auto item = _items.begin();
    while (item != _items.end()) {
        if ((*item)->used())
            _items.erase(item);
        else
            ++item;
    }

    auto npc = _npcs.begin();
    while (npc != _npcs.end()) {
        if ((*npc)->getHp() <= 0)
            _npcs.erase(npc);
        else
            ++npc;
    }
}

void Level::clear() {
    _player->clearBuff();
    _player->compass() = false;
    _player->setHp(getStat(_player->race()).hp);
    _tiles.clear();
    _chambers.clear();
    _npcs.clear();
    _items.clear();
}

Vec2D Level::getSpawnPos(Game &g) const {
    int chamber = g.randomInt(num_of_chamber());
    int n = g.randomInt(num_of_tile(chamber));
    Vec2D pos = nth_tile(chamber, n);
    while (getTile(pos) != Tile::floor || getItem(pos) || getNPC(pos) ||
           pos == _player->getPos()) {
        chamber = g.randomInt(num_of_chamber());
        n = g.randomInt(num_of_tile(chamber));
        pos = nth_tile(chamber, n);
    }
    return pos;
}

int Level::num_of_chamber() const { return _chambers.size(); }
int Level::num_of_tile(int chamber) const { return _chambers[chamber].size(); }
Vec2D Level::nth_tile(int chamber, int n) const {
    for (auto &pos : _chambers[chamber]) {
        if (n <= 0) return pos;
        --n;
    }
    throw "Index error";
}
int Level::isIn(Vec2D pos) const {
    int n = 0;
    for (auto &c : _chambers) {
        if (c.find(pos) != c.end()) return n;
        ++n;
    }
    return -1;
}

static inline void canReach(std::vector<std::vector<Tile>> &map,
                            std::set<Vec2D> &reached, Vec2D pos, Vec2D dir) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= int(map[0].size()) ||
        pos.y >= int(map.size()))
        return;
    if (reached.find(pos) == reached.end() && map[pos.y][pos.x] == Tile::floor)
        reached.insert(pos);
    else
        return;
    Vec2D next{0, 0};
    if (dir != Dir::R) {
        next = pos + Dir::R;
        canReach(map, reached, next, pos - next);
    }
    if (dir != Dir::D) {
        next = pos + Dir::D;
        canReach(map, reached, next, pos - next);
    }
    if (dir != Dir::L) {
        next = pos + Dir::L;
        canReach(map, reached, next, pos - next);
    }
    if (dir != Dir::U) {
        next = pos + Dir::U;
        canReach(map, reached, next, pos - next);
    }
}
