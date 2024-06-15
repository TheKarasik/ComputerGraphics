
#include "Game.h"

int main(int argc, char* argv[])
{
    /*Importer3D importer = Importer3D();
    importer.DoTheImportThing("Models/source/Maxwell.fbx");*/
    Game game = Game();
    game.GameLoop();
    return 0;
}
