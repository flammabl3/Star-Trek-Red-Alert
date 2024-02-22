#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Game {
    private:
        sf::RenderWindow* window;
        sf::VideoMode videoMode;
        sf::Event event;

        void initVariables();
        void initWindow();

    public:
        Game();
        virtual ~Game();

        const bool getWindowIsOpen() const;

        void updateEvents();
        void update();
        void render();
};