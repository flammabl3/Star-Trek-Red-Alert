#include <SFML/Graphics.hpp>

class Button {
    public:
        enum buttonActions {
            START,
            SETTINGS,
            EXIT,
            NONE
        };

        buttonActions action;
        sf::RectangleShape buttonRectangle;
        sf::Text buttonText;

        Button(int offset, std::string text, Button::buttonActions action, sf::View& view, sf::Font& font);
};