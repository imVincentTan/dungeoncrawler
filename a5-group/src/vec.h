#ifndef VEC_H
#define VEC_H

#include <climits>
#include <cmath>
#include <iostream>
#include <string>

enum class Dir {
    UL = 0,
    U = 1,
    UR = 2,
    L = 3,
    R = 4,
    DL = 5,
    D = 6,
    DR = 7,
    Null = -1
};

struct Vec2D {
    int x;
    int y;
    Vec2D()
        : x{0}
        , y{0} {}
    Vec2D(int x, int y)
        : x{x}
        , y{y} {}
    Vec2D(Dir dir) {
        if (dir == Dir::UL || dir == Dir::L || dir == Dir::DL)
            x = -1;
        else if (dir == Dir::U || dir == Dir::D)
            x = 0;
        else
            x = 1;
        if (dir == Dir::UL || dir == Dir::U || dir == Dir::UR)
            y = -1;
        else if (dir == Dir::L || dir == Dir::R)
            y = 0;
        else
            y = 1;
    }

    Vec2D &operator+=(const Vec2D &o) {
        x += o.x;
        y += o.y;
        return *this;
    }
    Vec2D &operator-=(const Vec2D &o) {
        x -= o.x;
        y -= o.y;
        return *this;
    }
    bool operator<(const Vec2D &o) const {
        if (y < o.y)
            return true;
        else if (y == o.y && x < o.x)
            return true;
        else
            return false;
    }
    bool operator==(const Vec2D &o) { return x == o.x && y == o.y; }
    bool operator!=(const Vec2D &o) { return !(*this == o); }
};

static inline Vec2D operator+(Vec2D a, const Vec2D &b) { return a += b; }
static inline Vec2D operator-(Vec2D a, const Vec2D &b) { return a -= b; }
static inline bool isAdjacent(Vec2D a, Vec2D b) {
    Vec2D diff = a - b;
    return diff.x <= 1 && diff.x >= -1 && diff.y <= 1 && diff.y >= -1;
}

static inline std::ostream &operator<<(std::ostream &out, Dir dir) {
    switch (dir) {
    case Dir::UL:
        return out << "North West";
    case Dir::U:
        return out << "North";
    case Dir::UR:
        return out << "North East";
    case Dir::L:
        return out << "West";
    case Dir::R:
        return out << "East";
    case Dir::DL:
        return out << "South West";
    case Dir::D:
        return out << "South";
    case Dir::DR:
        return out << "South East";
    default:
        throw "Wrong dir";
    }
}

static inline Dir dir(Vec2D vec) {
    if (vec.y == -1) {
        if (vec.x == -1) {
            return Dir::UL;
        } else if (vec.x == 0) {
            return Dir::U;
        } else if (vec.x == 1) {
            return Dir::UR;
        } else
            throw "Wrong vec";
    } else if (vec.y == 0) {
        if (vec.x == -1) {
            return Dir::L;
        } else if (vec.x == 1) {
            return Dir::R;
        } else
            throw "Wrong vec";
    } else if (vec.y == 1) {
        if (vec.x == -1) {
            return Dir::DL;
        } else if (vec.x == 0) {
            return Dir::D;
        } else if (vec.x == 1) {
            return Dir::DR;
        } else
            throw "Wrong vec";
    } else
        throw "Wrong vec";
}
#endif
