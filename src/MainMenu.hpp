#include <SFML/Graphics.hpp>
class MainMenu {
    public:
        std::vector<sf::RectangleShape> buttons;

        MainMenu();
        void initSFMLObjects();
};