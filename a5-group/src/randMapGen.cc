#include "randMapGen.h"

#include "vec.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static const std::vector<Dir> DIRS{Dir::R, Dir::D, Dir::L, Dir::U};

// random number from [start, end)
int randomInt(int start, int end) {
    return start + (rand() % (end - start + 1));
}

void roomgen(std::vector<std::string> &mapvec, int mapWidth, int mapHeight,
             int roomMaxWidth, int roomMaxHeight) {
    // note: rooms must have minimum have width and height of 1 occupiable
    // space, including walls have 3 spaces in length
    int numrooms = 8;
    int maxRoomgenAttempt = 10;
    int roomgenAttempt = 0;
    int tempy, tempx;
    int tempsizey, tempsizex;
    bool badroom = false;
    int mindim = 5;
    int roomcount = 0;
    bool isnewroom = true;
    while (roomcount < numrooms && roomgenAttempt < maxRoomgenAttempt) {
        isnewroom = true;
        badroom = false;
        // size of the room
        tempsizey = randomInt(mindim, roomMaxHeight);
        tempsizex = randomInt(mindim, roomMaxWidth);

        // top left corner of new room
        tempy = randomInt(2, mapHeight - tempsizey - 2);
        tempx = randomInt(2, mapWidth - tempsizex - 2);

        // check if theres any wall sharing
        for (int a = tempy; a < tempy + tempsizey; ++a) {
            badroom = badroom || (mapvec[a][tempx] == '|');
            badroom = badroom || (mapvec[a][tempx + tempsizex - 1] == '|');
        }
        for (int b = tempx; b < tempx + tempsizex; ++b) {

            badroom = badroom || (mapvec[tempy][b] == '-');
            badroom = badroom || (mapvec[tempy + tempsizey - 1][b] == '-');
        }

        if (!badroom) {
            // draw box
            for (int a = tempy; a < tempy + tempsizey; ++a) {
                for (int b = tempx; b < tempx + tempsizex; ++b) {
                    if (mapvec[a][b] == '.') isnewroom = false;
                    if ((b == tempx || b == tempx + tempsizex - 1) &&
                        mapvec[a][b] != '.') {
                        mapvec[a][b] = '|';
                    } else if ((a == tempy || a == tempy + tempsizey - 1) &&
                               mapvec[a][b] != '.') {
                        if (mapvec[a][b] != '|') mapvec[a][b] = '-';
                    } else {
                        mapvec[a][b] = '.';
                    }
                }
            }
            ++roomgenAttempt;
            if (isnewroom) {
                ++roomcount;
            }
        }
    }
    /*special edgecase:
        |.
        .|
    or
        .|
        |.
    */
    for (int a = 1; a < mapHeight - 1; ++a) {
        for (int b = 1; b < mapWidth - 1; ++b) {
            if (mapvec[a][b] == '|' && mapvec[a][b + 1] == '.' &&
                mapvec[a + 1][b] == '.' && mapvec[a + 1][b + 1] == '|') {
                mapvec[a][b] = mapvec[a + 1][b + 1] = '.';
                mapvec[a - 1][b - 1] = mapvec[a][b - 1] = mapvec[a + 1][b + 2] =
                    mapvec[a + 2][b + 2];
            } else if (mapvec[a][b] == '.' && mapvec[a][b + 1] == '|' &&
                       mapvec[a + 1][b] == '|' && mapvec[a + 1][b + 1] == '.') {
                mapvec[a][b + 1] = mapvec[a + 1][b] = '.';
                mapvec[a - 1][b + 2] = mapvec[a][b + 2] = mapvec[a + 1][b - 1] =
                    mapvec[a + 2][b - 1] = '|';
            }
        }
    }
}

// helper
std::pair<int, int> charsearch(std::vector<std::string> &mapvec, int mapWidth,
                               int mapHeight, char target,
                               std::set<std::pair<int, int>> ignorelist) {
    for (int a = 1; a < mapHeight - 1; ++a) {
        for (int b = 1; b < mapWidth - 1; ++b) {
            if ((mapvec[a][b] == target) &&
                not(ignorelist.find(std::make_pair(a, b)) !=
                    ignorelist.end())) {
                return std::make_pair(a, b);
            }
        }
    }
    return std::make_pair(0, 0);
}

// helper for adddoor
void walldots(std::vector<std::string> &mapvec, int mapWidth, int mapHeight,
              int thisx, int thisy, std::set<std::pair<int, int>> &ignorelist,
              std::set<std::pair<int, int>> &potdoors) {
    // if this dot is not in the ignorelist
    if (not(ignorelist.find(std::make_pair(thisy, thisx)) !=
            ignorelist.end())) {
        // if this dot is a candidate for being next to a door
        if (mapvec[thisy - 1][thisx] == '-' && mapvec[thisy - 2][thisx] == '#')
            potdoors.emplace(std::make_pair(thisy - 1, thisx));
        if (mapvec[thisy][thisx - 1] == '|' && mapvec[thisy][thisx - 2] == '#')
            potdoors.emplace(std::make_pair(thisy, thisx - 1));
        if (mapvec[thisy][thisx + 1] == '|' && mapvec[thisy][thisx + 2] == '#')
            potdoors.emplace(std::make_pair(thisy, thisx + 1));
        if (mapvec[thisy + 1][thisx] == '-' && mapvec[thisy + 2][thisx] == '#')
            potdoors.emplace(std::make_pair(thisy + 1, thisx));

        ignorelist.emplace(std::make_pair(thisy, thisx));

        if (mapvec[thisy - 1][thisx] == '.')
            walldots(mapvec, mapWidth, mapHeight, thisx, thisy - 1, ignorelist,
                     potdoors);
        if (mapvec[thisy][thisx - 1] == '.')
            walldots(mapvec, mapWidth, mapHeight, thisx - 1, thisy, ignorelist,
                     potdoors);
        if (mapvec[thisy][thisx + 1] == '.')
            walldots(mapvec, mapWidth, mapHeight, thisx + 1, thisy, ignorelist,
                     potdoors);
        if (mapvec[thisy + 1][thisx] == '.')
            walldots(mapvec, mapWidth, mapHeight, thisx, thisy + 1, ignorelist,
                     potdoors);
    }
}

std::set<Vec2D> adddoor(std::vector<std::string> &mapvec, int mapWidth,
                        int mapHeight) {
    std::set<Vec2D> retval;
    std::set<std::pair<int, int>> ignorelist; // set of coordinates of dots
    std::set<std::pair<int, int>>
        potdoors; // set of coordinates of potential doors (the actual walls)

    while (charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist) !=
           std::make_pair(0, 0)) {
        int tempy =
            charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist).first;
        int tempx =
            charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist).second;

        walldots(mapvec, mapWidth, mapHeight, tempx, tempy, ignorelist,
                 potdoors);

        int potdoorssize = potdoors.size();
        int numdoors =
            randomInt(1, std::min(3, potdoorssize)); // for a particular room

        for (int counter = 0; counter < numdoors; ++counter) {
            potdoorssize = potdoors.size();
            int temprand = randomInt(0, potdoorssize - 1);
            std::pair<int, int> temppair =
                *std::next(potdoors.begin(), temprand);
            int doory = temppair.first;
            int doorx = temppair.second;
            mapvec[doory][doorx] = '+';
            retval.emplace(doorx, doory);
            potdoors.erase(temppair);
        }
        potdoors.clear();
    }
    return retval;
}

std::pair<std::vector<std::string>, std::set<Vec2D>> genmap(int mapWidth,
                                                            int mapHeight) {
    /*
    Steps:
    1) default set the map to spaces and border characters
    2) randomly generate rooms
    3) randomly add doors (+)
    4) return to pathway generator (remember to include whitespace characters in
    the string)
    */

    std::vector<std::string> mapvec;
    mapvec = std::vector<std::string>(mapHeight, std::string(mapWidth, '#'));
    int roomMaxWidth = 15;
    int roomMaxHeight = 10;

    // 1) default set the map to spaces and border characters
    for (int x = 0; x < mapWidth; ++x) {
        mapvec[0][x] = '-';
        mapvec[mapHeight - 1][x] = '-';
    }
    for (int y = 0; y < mapHeight; ++y) {
        mapvec[y][0] = '|';
        mapvec[y][mapWidth - 1] = '|';
    }

    // 2) randomly generate rooms
    roomgen(mapvec, mapWidth, mapHeight, roomMaxWidth, roomMaxHeight);

    // 3) randomly add doors (+)
    std::set<Vec2D> doors = adddoor(mapvec, mapWidth, mapHeight);

    // 4) return as a string to be used in game (remember to include whitespace
    // characters in the string)
    return make_pair(mapvec, doors);
}

// adding pathways:

std::set<Vec2D> canReach(std::vector<std::string> &map,
                         std::set<Vec2D> &reached, Vec2D pos, Vec2D dir) {
    std::set<Vec2D> reachable;
    if (reached.find(pos) == reached.end())
        reached.insert(pos);
    else
        return reachable;
    Vec2D next{0, 0};
    for (auto &t_dir : DIRS) {
        if (dir != t_dir) {
            next = pos + t_dir;
            if (next.x > 0 && next.y > 0 && next.x < int(map[0].size()) &&
                next.y < int(map.size())) {
                if (map[next.y][next.x] == '+')
                    reachable.emplace(next);
                else if (map[next.y][next.x] == '#') {
                    auto reach = canReach(map, reached, next, pos - next);
                    reachable.insert(reach.begin(), reach.end());
                }
            }
        }
    }
    return reachable;
}

static inline bool near(std::vector<std::string> &map, Vec2D p, char n) {
    char c = map[p.y][p.x];
    return c == n - 1 || c == '-' || c == '|';
}
static inline bool inRange(std::vector<std::string> &map, Vec2D p) {
    return p.x >= 0 && p.y >= 0 && p.x < int(map[0].size()) &&
           p.y < int(map.size());
}

bool candidate(std::vector<std::string> &map, Vec2D pos, char n) {
    for (auto &dir : DIRS) {
        Vec2D next = pos + dir;
        if (inRange(map, next) && near(map, next, n)) return true;
    }
    return false;
}

static inline bool tryRemove(std::vector<std::string> &map, Vec2D pos,
                             int total, Vec2D p, char n) {
    int x = p.x;
    int y = p.y;
    if (map[y][x] == '#' && candidate(map, p, n)) {
        std::set<Vec2D> reached;
        map[y][x] = n;
        if (int(canReach(map, reached, pos, pos).size()) == total)
            return true;
        else
            map[y][x] = '#';
    }
    return false;
}

std::string randMap() {
    srand(time(NULL));
    int width = 79;
    int height = 25;
    std::ostringstream retval;
    auto mapanddoor = genmap(width, height); // hard code dimentions
    std::vector<std::string> map = mapanddoor.first;
    std::set<Vec2D> doors = mapanddoor.second;

    bool removed = true;
    int total = doors.size() - 1;
    Vec2D pos = *doors.begin();
    char n = 0;
    while (removed) {
        removed = false;
        int x_left = 0;
        int x_right = width;
        int y_left = 0;
        int y_right = height;
        int x, y;
        while (x_left < x_right && y_left < y_right) {
            x = x_left;
            y = y_left;
            while (x < x_right) {
                removed |= tryRemove(map, pos, total, {x, y}, n);
                ++x;
            }
            --x;
            ++y;
            ++y_left;
            while (y < y_right) {
                removed |= tryRemove(map, pos, total, {x, y}, n);
                ++y;
            }
            --y;
            --x;
            --x_right;
            while (x_left <= x) {
                removed |= tryRemove(map, pos, total, {x, y}, n);
                --x;
            }
            ++x;
            --y;
            --y_right;
            while (y_left <= y) {
                removed |= tryRemove(map, pos, total, {x, y}, n);
                --y;
            }
            ++x_left;
        }
        ++n;
        if (n > 31) n = 0;
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] < 32) map[y][x] = ' ';
        }
    }

    for (auto &line : map) {
        retval << line << std::endl;
    }
    return retval.str();
}
