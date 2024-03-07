#include "Game.hpp"

#include <iostream>
#include <cmath>
#include <windows.h>

#include "ship.hpp"
#include "NCC-1701-D.hpp"
#include "Projectile.hpp"

void Game::initVariables() {
    this->window = nullptr;
    this->weaponSelected = false;
}

void Game::initWindow() {
    this->videoMode.width = 800;
    this->videoMode.height = 600;
    this->window = new sf::RenderWindow(sf::VideoMode(800, 600), "Star Trek: Red Alert");
    this->window->setKeyRepeatEnabled(false);

    sf::Vector2u size = this->window->getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

}

void Game::initPlayer() {
    if (!this->playerTexture.loadFromFile("../resource/Ent-D.png")) {
        std::cout << "Failed to load." << std::endl;
    }
    
    this->playerShip.setTexture(this->playerTexture);
    this->playerShip.setPosition(400, 300);
    Ship enterprise = getEnterprise();
}

void Game::updatePlayer() {
    this->movePlayer();
    this->fireWeapon(playerShip);
}

void Game::renderPlayer() {
    this->window->draw(this->playerShip);
}

void Game::renderProjectiles() {
    for (int i = 0; i < projectilesList.size(); i++) {
        projectilesList.at(i)->render(this->window);
    }
}

// will be run each frame. will eventually need code to check the type of weapon.

//TODO: The torpedo also has no code to move.
void Game::fireWeapon(sf::Sprite firingShip) {
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && weaponSelected){
        Projectile* torpedo = new Projectile("../resource/photontorpedo.png", firingShip.getPosition());

        this->projectilesList.insert(projectilesList.end(), torpedo);
        weaponSelected = false;
    }
}

void Game::movePlayer() {
    // these variables, and this function itself should eventually be moved to the Ship class.
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !weaponSelected) {
        /* Get the position of the player's ship and the position of the mouse as vectors. 
        Find the vector which is the difference between the 2 vectors and normalize it by dividing by length. The vector is normalized so it can be multiplied by a constant speed.
        Move by the difference vector times speed times deltatime. */
        mousePosition = sf::Mouse::getPosition(*window);
        sf::Clock clock;
        sf::Vector2f movementDistance = (sf::Vector2f) mousePosition - playerShip.getPosition();
        float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
        
        if (length != 0.0f) {
            sf::Vector2f normalizedSpeed = movementDistance / length;

            std::cout << "" << std::endl; //why does this print statement make the code work??
            float deltaTime = clock.restart().asSeconds();
            float speed = 100.0f; // this should be replaced by the top speed of the ship.
            playerShip.move(normalizedSpeed * speed * deltaTime);

            /*Find the angle of the distance vector using atan2, and convert to degrees. Then normalize it to be from 0 to 360 degrees. */
            float distanceAngle = (180.0f / M_PI * atan2(normalizedSpeed.y, normalizedSpeed.x));
            distanceAngle = std::fmod(distanceAngle + 360.0f, 360.0f);
            //Algorithm for determining if it is closer to rotate clockwise or counterclockwise to the target angle. 
            //Will not trigger if the ship's orientation is within 10 degrees of where the user is currently clicking.
            int cwDistance;
            int ccwDistance;
            if (abs(playerShip.getRotation() - distanceAngle) > 10) {
                if (distanceAngle >= playerShip.getRotation()) {
                    cwDistance = distanceAngle - playerShip.getRotation();
                    ccwDistance = playerShip.getRotation() + 360.0f - distanceAngle;
                } else {
                    cwDistance = 360.0f - playerShip.getRotation() + distanceAngle;
                    ccwDistance = playerShip.getRotation() - distanceAngle;
                }

                if (ccwDistance > cwDistance) {
                    playerShip.rotate(100 * deltaTime);
                } else if (ccwDistance < cwDistance) {
                    playerShip.rotate(-100 * deltaTime);
                }
               
            }
            
        }
        
    }
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
    while (this->window->pollEvent(this->event)) {
        if (this->event.type == sf::Event::Closed) {
            this->window->close();
            exit(0);
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.scancode == sf::Keyboard::Scan::Num1)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                weaponSelected = true;
                std::cout << "weapon 1 selected" << std::endl;
            }
        }
    }
}

void Game::update() {
    this->window->clear();
    this->updateEvents();
    this->updatePlayer();
}

void Game::render() {
    renderProjectiles();
    renderPlayer();
    this->window->display();
}