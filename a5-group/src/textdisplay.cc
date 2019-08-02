#include "textdisplay.h"

#include "game.h"
#include "item.h"
#include "npc.h"
#include "player.h"
#include "potion.h"
#include "vec.h"

#include <map>
#include <sstream>
#include <string>

TextDisplay::TextDisplay(Vec2D size)
    : theDisplay{std::string{}}
    , size{size} {}

void TextDisplay::refresh(Game &g) {
    printMap(g, theDisplay);
    printInfo(g, theDisplay);
    std::cout << theDisplay;
}

void TextDisplay::getInput(Game &g) {
    Action &action = g.action();
    std::string s;

    while (true) {
        std::cin >> s;
        if (s == "r") {
            action.act = Act::start;
            return;
        } else if (s == "q") {
            action.act = Act::quit;
            return;
        } else if (s[0] == 'u') {
            action.act = Act::use;
            s.erase(0, 1);
        } else if (s[0] == 'a') {
            action.act = Act::attack;
            s.erase(0, 1);
        } else {
            action.act = Act::move;
        }
        if (s == "ea")
            action.dir = Dir::R;
        else if (s == "we")
            action.dir = Dir::L;
        else if (s == "no")
            action.dir = Dir::U;
        else if (s == "ne")
            action.dir = Dir::UR;
        else if (s == "nw")
            action.dir = Dir::UL;
        else if (s == "so")
            action.dir = Dir::D;
        else if (s == "se")
            action.dir = Dir::DR;
        else if (s == "sw")
            action.dir = Dir::DL;
        else
            continue;
        break;
    }
}

void TextDisplay::printStats(Game &g) {
    Player &p = g.player();
    if (p.getHp() > 0) {
        std::cout << "You WIN!" << std::endl;
    } else {
        std::cout << "Game over!" << std::endl;
    }
    double score = p.getGold() / 10.0;
    if (p.race() == Race::human) score += score / 2;
    std::cout << "Your score is: " << score << std::endl;
}

Race TextDisplay::selectRace() {
    char c;
    while (true) {
        std::cout << "Please select your Race (h/d/e/o): ";
        std::cin >> c;
        switch (c) {
        case 'h':
            return Race::human;
        case 'd':
            return Race::dwarf;
        case 'e':
            return Race::elves;
        case 'o':
            return Race::orc;
        }
    }
}

bool TextDisplay::restart() {
    char c;
    while (true) {
        std::cout << "Do you want to restart? (r/q): ";
        std::cin >> c;
        switch (c) {
        case 'r':
            return true;
        case 'q':
            return false;
        }
    }
}

void TextDisplay::quit() { std::cout << "Thanks for playing!" << std::endl; }

static inline char getNPCChar(Race race);
static inline char getTileChar(Tile tile);
static inline char getItemChar(std::vector<std::string> desc);

void TextDisplay::printMap(Game &g, std::string &disp) {
    std::ostringstream oss;
    Level &level = g.level();
    Player &p = g.player();
    const auto &tiles = level.tiles();
    const auto &npcs = level.npcs();
    const auto &items = level.items();

    for (auto row : tiles) {
        for (auto tile : row) {
            if (tile == Tile::stair) {
                if (p.compass())
                    oss << getTileChar(tile);
                else if (g.debug())
                    oss << '/';
                else
                    oss << getTileChar(Tile::floor);
            } else {
                oss << getTileChar(tile);
            }
        }
        oss << std::endl;
    }

    Vec2D pos = p.getPos();
    disp = oss.str();
    disp[pos.y * (level.width() + 1) + pos.x] = '@';

    for (auto &item : items) {
        if (item->used()) continue;
        pos = item->getPos();
        disp[pos.y * (level.width() + 1) + pos.x] = getItemChar(item->isA());
    }

    for (auto &npc : npcs) {
        if (npc->getHp() <= 0) continue;
        pos = npc->getPos();
        disp[pos.y * (level.width() + 1) + pos.x] = getNPCChar(npc->race());
    }
}

static inline std::string getPotionName(PotionType type);
static inline std::string potionLine(Player &p, std::vector<Item *> items);

void TextDisplay::printInfo(Game &g, std::string &disp) {
    std::ostringstream oss;
    Level &level = g.level();
    Player &p = g.player();
    auto race = p.isA()[1];

    oss << "Race: " << race << " Gold: " << p.getGold() / 10.0;
    oss << std::string(69 - oss.str().length(), ' ') << "Floor "
        << level.level() << std::endl
        << "HP: " << p.getHp() << std::endl
        << "Atk: " << p.getAtk() << std::endl
        << "Def: " << p.getDef() << std::endl;

    oss << "Action:";
    Action action = g.action();
    Act act = action.act;
    Dir dir = action.dir;
    Character *c = action.actee;
    Item *item = action.item;
    if (act == Act::move) {
        oss << " Player ";
        if (action.dir != Dir::Null) {
            oss << "moves " << dir
                << potionLine(p, level.itemAround(p.getPos()));
        } else {
            if (c)
                oss << "bumps into a " << c->isA()[1];
            else if (item)
                oss << "bumps into a " << item->isA()[1];
            else
                oss << "bumps into a wall";
        }
        oss << ".";
    } else if (act == Act::attack) {
        if (c) {
            if (c->getHp() <= 0) {
                oss << " PC killed " << getNPCChar(c->race()) << ".";
            } else {
                oss << " PC deals " << action.num;
                oss << " damage to " << getNPCChar(c->race());
                oss << " (" << c->getHp() << " HP).";
            }
        } else {
            oss << " PC didn't see any monster there.";
        }
    } else if (act == Act::use) {
        if (item)
            oss << " PC uses " << item->isA()[2] << ".";
        else
            oss << " PC didn't see any item there.";
    } else if (act == Act::start) {
        oss << " Player character has spawned.";
    }

    for (auto action : g.npc_action()) {
        if (action.act == Act::attack) {
            c = action.actor;
            int damage = action.num;
            oss << " " << getNPCChar(c->race());
            if (damage > 0)
                oss << " deals " << damage << " damage to PC.";
            else
                oss << " attacks PC but missed.";
        }
    }
    oss << std::endl;

    disp += oss.str();
}

static inline char getNPCChar(Race race) {
    switch (race) {
    case Race::vampire:
        return 'V';
    case Race::werewolf:
        return 'W';
    case Race::troll:
        return 'T';
    case Race::goblin:
        return 'N';
    case Race::merchant:
        return 'M';
    case Race::dragon:
        return 'D';
    case Race::phoenix:
        return 'X';
    default:
        throw "Unknown Race";
    }
}

static inline char getTileChar(Tile tile) {
    switch (tile) {
    case Tile::empty:
        return ' ';
    case Tile::v_wall:
        return '|';
    case Tile::h_wall:
        return '-';
    case Tile::floor:
        return '.';
    case Tile::door:
        return '+';
    case Tile::passage:
        return '#';
    case Tile::stair:
        return '\\';
    default:
        throw "Unknown Tile";
    }
}

static inline char getItemChar(std::vector<std::string> desc) {
    if (desc[1] == "Potion") {
        return 'P';
    } else if (desc[1] == "Treasure") {
        return 'G';
    } else if (desc[2] == "Barrier Suit") {
        return 'B';
    } else if (desc[2] == "Compass") {
        return 'C';
    } else {
        throw "Unknown Item";
    }
}

static inline std::string getPotionName(PotionType type) {
    if (type == PotionType::RH) {
        return "RH";
    } else if (type == PotionType::BA) {
        return "BA";
    } else if (type == PotionType::BD) {
        return "BD";
    } else if (type == PotionType::PH) {
        return "PH";
    } else if (type == PotionType::WA) {
        return "WA";
    } else if (type == PotionType::WD) {
        return "WD";
    } else {
        throw "Unknown Potion";
    }
}

static inline std::string potionLine(Player &p, std::vector<Item *> items) {
    std::ostringstream oss;
    std::map<PotionType, int> count;
    PotionType type;
    for (auto &item : items) {
        auto is_a = item->isA();
        if (is_a[1] == "Potion") {
            type = ((Potion *)item)->type();
            ++count[type];
        }
    }
    if (count.size() == 0) {
        return oss.str();
    } else if (count.size() == 1 && count.begin()->second == 1) {
        oss << " and sees a";
        if (p.learnedPotion(type))
            oss << " " << getPotionName(type);
        else
            oss << "n unknown potion";
    } else {
        int unknown = 0;
        int printed = 0;
        oss << " and sees";
        auto type = count.begin();
        auto end = type;
        if (type != count.end()) {
            while (++type != count.end()) {
                if (p.learnedPotion(end->first)) {
                    oss << " " << end->second << " "
                        << getPotionName(end->first);
                    ++printed;
                } else {
                    unknown += end->second;
                }
                end = type;
            }
        }
        if (p.learnedPotion(end->first)) {
            if (!unknown && printed) oss << " and";
            oss << " " << end->second << " " << getPotionName(end->first);
        } else {
            unknown += end->second;
        }
        if (unknown > 0) {
            if (printed) oss << " and";
            oss << " " << unknown << " unknown potion";
        }
    }
    return oss.str();
}
