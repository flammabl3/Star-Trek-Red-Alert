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
    //float deltaTime = clock.restart().asSeconds();
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
    playerShipObj = getEnterprise(); // The Ship object associated with the player's ship will be made the USS enterprise using a function from NCC-1701-D.hpp.
    playerShip.setOrigin(playerShip.getLocalBounds().width / 2, playerShip.getLocalBounds().height / 2);
    playerShipObj.shipSprite = playerShip;
}

//this is not necessary as we can use playerShip.getLocalBounds() and avoid using this size variable. Leave it here anyways in case it is needed later.
void Game::sizePlayerCheck() {
        //vector of ship size. 
    sf::Vector2f playerSize(
    playerShip.getTexture()->getSize().x * playerShip.getScale().x,
    playerShip.getTexture()->getSize().y * playerShip.getScale().y);

    playerShipObj.setSize(playerSize.x, playerSize.y, 0); // no height for now.
}

void Game::updatePlayer() {
    this->sizePlayerCheck();
    this->movePlayer();
    this->fireWeapon(playerShipObj);
}

void Game::renderPlayer() {
    this->window->draw(this->playerShip);
}

void Game::renderProjectiles() {
    for (int i = 0; i < projectilesList.size(); i++) {
        projectilesList.at(i)->render(this->window);
        moveProjectiles(projectilesList.at(i), i);
        //this function belongs to the Projectile class.
    }
}

void Game::moveProjectiles(Projectile* projectile, int i) {
    sf::Vector2f elapsedDistance = projectile->projectileSprite.getPosition() - projectile->spawnedAt;
    if (std::sqrt(elapsedDistance.x * elapsedDistance.x + elapsedDistance.y * elapsedDistance.y) > 400) {
        projectilesList.erase(projectilesList.begin() + i);
        delete projectile;
        projectile = nullptr;
    } else {

        sf::Vector2f directionOfTravel;
        //A vector with magnitude one pointing in the direction of the projectile.

        directionOfTravel.x = cos(projectile->projectileSprite.getRotation() * M_PI/180);
        directionOfTravel.y = sin(projectile->projectileSprite.getRotation() * M_PI/180);
        float length = std::sqrt(directionOfTravel.x * directionOfTravel.x + directionOfTravel.y * directionOfTravel.y);
        directionOfTravel = directionOfTravel / length;

        projectile->projectileSprite.move(directionOfTravel * projectile->speed * deltaTime);

    }
}

// will be run each frame. will eventually need code to check the type of weapon.
void Game::fireWeapon(Ship firingShip) {
    //need to constantly update the sprite object in the Ship object. That's annoying.
    playerShipObj.shipSprite = playerShip;

    sf::Vector2f parentTip = playerShip.getTransform().transformPoint({playerShip.getLocalBounds().height, playerShip.getLocalBounds().height / 2});

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && weaponSelected){
        Projectile* torpedo = new Projectile("../resource/photontorpedo.png", parentTip.x, parentTip.y,
                                            firingShip.shipSprite.getRotation(), 1000.0);                                  
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
        sf::Vector2f movementDistance = (sf::Vector2f) mousePosition - playerShip.getPosition();
        float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
        
        if (length != 0.0f) {
            sf::Vector2f normalizedVector = movementDistance / length;
           

            float speed = 50.0f; // this should be replaced by the top speed of the ship.
            playerShip.move(normalizedVector * speed * deltaTime);


            /*Find the angle of the distance vector using atan2, and convert to degrees. Then normalize it to be from 0 to 360 degrees. */
            float distanceAngle = (180.0f / M_PI * atan2(normalizedVector.y, normalizedVector.x));
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
                    playerShip.rotate(70 * deltaTime);
                } else if (ccwDistance < cwDistance) {
                    playerShip.rotate(-70 * deltaTime);
                }
               
            }
            
        }
        
    }
}

Game::Game() {
    this->initVariables();
    this->initWindow();
    this->initPlayer();
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
    deltaTime = clock.restart().asSeconds();
    this->window->clear();
    this->updateEvents();
    this->updatePlayer();
}

void Game::render() {
    renderProjectiles();
    renderPlayer();
    this->window->display();
}