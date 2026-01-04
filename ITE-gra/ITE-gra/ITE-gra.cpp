#include "game.h"

int main() {
    Game game;

    if (game.initialize()) {
        game.run();
    }

    game.cleanup();
    return 0;
}