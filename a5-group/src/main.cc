#include "game.h"
#include "textdisplay.h"
#include "vec.h"

#include <ctime>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
    std::string file = "floor.txt";
    auto seed = unsigned(std::time(0));
    bool debug = false;
    bool enhance = false;
    auto disp = std::make_unique<TextDisplay>(Vec2D{80, 30});
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (std::string{argv[i]} == "-setrng") {
                seed = 0;
            } else if (std::string{argv[i]} == "-debug") {
                seed = 0;
                debug = true;
                enhance = true;
            } else if (std::string{argv[i]} == "-enhance") {
                enhance = true;
            } else {
                file = argv[1];
            }
        }
    }
    Game game{std::vector<Display *>{disp.get()}, file, seed, enhance, debug};
    game.start();
}
