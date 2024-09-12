#include "Menu.hpp"

MainMenu::MainMenu() {
}

Menu::Menu() {

}

void MainMenu::initSFMLObjects(sf::View view) {
    std::shared_ptr<sf::RectangleShape> exit = std::make_shared<sf::RectangleShape>(sf::RectangleShape());
    exit->setSize(sf::Vector2f(200, 100));
    exit->setOrigin(100, 50);
    exit->setPosition(view.getCenter().x, view.getCenter().y - 100);
    exit->setFillColor(sf::Color::Red);
    // Store in the inherited `buttons` map
    buttons.push_back(std::make_pair(menuActions::EXIT, exit));


    std::shared_ptr<sf::RectangleShape> start = std::make_shared<sf::RectangleShape>(sf::RectangleShape());
    start->setSize(sf::Vector2f(200, 100));
    start->setOrigin(100, 50);
    start->setPosition(view.getCenter().x, view.getCenter().y + 100);
    start->setFillColor(sf::Color::Red);
    buttons.push_back(std::make_pair(menuActions::START, start));
}

std::vector<std::pair<Menu::menuActions, std::shared_ptr<sf::RectangleShape>>> MainMenu::getButtonsAndItems() {
    return buttons;
}