#include "Button.hpp"

Button::Button(int offset, std::string text, Button::buttonActions action, sf::View& view, sf::Font& font) {
    buttonRectangle = sf::RectangleShape();
    buttonRectangle.setSize(sf::Vector2f(200, 100));
    buttonRectangle.setOrigin(100, 50);
    buttonRectangle.setPosition(view.getCenter().x, view.getCenter().y + offset);
    buttonRectangle.setFillColor(sf::Color::Red);
    
    buttonText = sf::Text(text, font);
    buttonText.setFillColor(sf::Color::White);
    sf::FloatRect rc = buttonText.getLocalBounds();
    buttonText.setOrigin(rc.width/2, rc.height/2);
    buttonText.setPosition(view.getCenter().x, view.getCenter().y + offset);

    this->action = action;
}