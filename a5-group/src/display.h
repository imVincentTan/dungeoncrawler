#ifndef DISPLAY_H
#define DISPLAY_H
class Game;
class Input;
enum class Race;

class Display {
  public:
    virtual ~Display() = default;

    virtual void refresh(Game &g) = 0;
    virtual void getInput(Game &g) = 0;
    virtual void printStats(Game &g) = 0;
    virtual Race selectRace() = 0;
    virtual bool restart() = 0;
    virtual void quit() = 0;
};
#endif
