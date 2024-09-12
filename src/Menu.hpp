#include "Button.hpp"
#include <memory>

class Menu {
    public:

        std::vector<std::shared_ptr<Button>> buttons;

        Menu();
        virtual void initSFMLObjects(sf::View& view, sf::Font& font) = 0;
        virtual std::vector<std::shared_ptr<Button>> getButtonsAndItems() = 0;
};

class MainMenu : public Menu {
    public:
        MainMenu();

        virtual void initSFMLObjects(sf::View& view, sf::Font& font) override;
        virtual std::vector<std::shared_ptr<Button>> getButtonsAndItems() override;
};

class PauseMenu : public Menu {

};