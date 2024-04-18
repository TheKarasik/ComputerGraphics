
#include "public/Game.h"

int main(int argc, char* argv[])
{
    Game game;
    game.Initialize();
    game.GameLoop();
    return 0;
}
