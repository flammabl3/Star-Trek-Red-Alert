#include "Game.h"

int main()
{
    Game game;

    game.initPlayer();

    while (game.getWindowIsOpen())
    {
        
        game.update();
        
        game.render();
    }

    return 0;
}