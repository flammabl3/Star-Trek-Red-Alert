#include "Game.hpp"

int main()
{
    Game game;

    while (game.getWindowIsOpen())
    {
        if (game.state == Game::GameState::MainMenu) {
            game.updateMainMenu();
            game.renderMainMenu();
        } else {
            game.update();
            
            game.render();
        }
    }

    return 0;
}