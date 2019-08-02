#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include "display.h"
#include "vec.h"

#include <string>

class TextDisplay final : public Display {
  public:
    TextDisplay(Vec2D size);

    void refresh(Game &g) override;
    void getInput(Game &g) override;
    void printStats(Game &g) override;
    Race selectRace() override;
    bool restart() override;
    void quit() override;

  private:
    std::string theDisplay;
    Vec2D size;
    void printMap(Game &g, std::string &disp);
    void printInfo(Game &g, std::string &disp);
};
#endif
