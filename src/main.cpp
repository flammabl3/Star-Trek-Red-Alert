#include "Game.hpp"

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