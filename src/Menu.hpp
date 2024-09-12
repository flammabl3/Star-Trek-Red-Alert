#include <SFML/Graphics.hpp>
#include <memory>

class Menu {
    public:
        enum menuActions {
            START,
            SETTINGS,
            EXIT,
            NONE
        };

        std::vector<std::pair<menuActions, std::shared_ptr<sf::RectangleShape>>> buttons;

        Menu();
        virtual void initSFMLObjects(sf::View view) = 0;
        virtual std::vector<std::pair<menuActions, std::shared_ptr<sf::RectangleShape>>> getButtonsAndItems() = 0;
};

class MainMenu : public Menu {
    public:
        MainMenu();

        virtual void initSFMLObjects(sf::View view) override;
        virtual std::vector<std::pair<menuActions, std::shared_ptr<sf::RectangleShape>>> getButtonsAndItems() override;
};

class PauseMenu : public Menu {

};