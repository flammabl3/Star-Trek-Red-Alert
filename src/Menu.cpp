#include "Menu.hpp"

MainMenu::MainMenu() {
}

Menu::Menu() {

}

void MainMenu::initSFMLObjects(sf::View& view, sf::Font& font) {
    std::shared_ptr<Button> exit = std::make_shared<Button>(Button(-100, "Quit Game", Button::buttonActions::EXIT, view, font));

    // Store in the inherited `buttons` vector
    buttons.push_back(exit);

    std::shared_ptr<Button> start = std::make_shared<Button>(Button(100, "Start Game", Button::buttonActions::START, view, font));
    buttons.push_back(start);
}

std::vector<std::shared_ptr<Button>> MainMenu::getButtonsAndItems() {
    return buttons;
}