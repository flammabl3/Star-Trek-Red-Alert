#include "Game.h"

void Game::initVariables() {
    this->window = nullptr;
}

void Game::initWindow() {
    this->videoMode.width = 800;
    this->videoMode.height = 600;
    this->window = new sf::RenderWindow(sf::VideoMode(800, 600), "Star Trek: Red Alert");
}

Game::Game() {
    this->initVariables();
    this->initWindow();
}

Game::~Game() {
    delete this->window;
}

const bool Game::getWindowIsOpen() const {
    return this->window->isOpen();
}


void Game::updateEvents() {
    while (this->window->pollEvent(this->event))
        {
            if (this->event.type == sf::Event::Closed)
                this->window->close();
        }
}

void Game::update() {
    this->updateEvents();
}

void Game::render() {
    this->window->clear();
    sf::Vector2u size = this->window->getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
    rectangle.setPosition(size.x / 2, size.y / 2);
    this->window->draw(rectangle);
    this->window->display();
}