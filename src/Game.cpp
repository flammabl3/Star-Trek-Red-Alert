#include "Game.hpp"

#include <iostream>
#include <cmath>
#include <windows.h>

#include "ship.hpp"
#include "NCC-1701-D.hpp"
#include "Projectile.hpp"



//TODO: See if we can't come up with a shorter way to reference the playerShipObj's shipSprite member
// Next steps: change the hitboxes to correctly account for rotation.
// Account for hull damage
// Create functions to check for and apply damage, and damage rooms, subsystems, and personnel
// Create some kind of enemy AI.
//
void Game::initVariables() {
    this->window = nullptr;
    this->weaponSelected = false;

    //pick a font later.
    font.loadFromFile("../resource/arial.ttf");

    debugMode = true;
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
    playerShipObj = getEnterprise(); // The Ship object associated with the player's ship will be made the USS enterprise using a function from NCC-1701-D.hpp.
    playerShipObj.setSFMLObjects("../resource/Ent-D.png"); // Call function to set texture and sprite.
    playerShipObj.shipSprite.setPosition(400, 300);
    playerShipObj.setFriendly();
    //create a pointer to reference our player ship object, and add it to the vector. This seems... non-optimal.
    Ship* playerShipPointer = &playerShipObj;
    allShips.push_back(playerShipPointer);
}

void Game::updatePlayer() {
    this->movePlayer();
    this->fireWeapon(playerShipObj);
}

void Game::renderPlayer() {
    playerShipObj.render(this->window);
}

void Game::renderDebugObjects() {
    for (sf::RectangleShape rectangle: debugHitboxes) {
        this->window->draw(rectangle);
    }
}

//placeholder code will generate another USS enterprise for shooting at.
void Game::initEnemy() {
    SATHelper sat; //using the same SATHelper multiple times throughout the code causes errors.
    Ship* enemyShipObj = getEnterprisePointer();
    enemyShipObj->setSFMLObjects("../resource/Ent-D.png");
    enemyShipObj->shipSprite.setPosition(300, 300);
    enemyShips.push_back(enemyShipObj);
    allShips.push_back(enemyShipObj);
    enemyShipObj->shipSprite.setRotation(0);

    debugHitboxes.push_back(enemyShipObj->returnHitbox());
    for (auto& pair : enemyShipObj->shipSystems) {
        System& system = pair.second;
        system.setHitbox(enemyShipObj);
        debugHitboxes.push_back(system.returnHitbox());
    }

    std::vector<sf::RectangleShape> normals = sat.returnNormals(enemyShipObj->shipSprite);
    for (auto& normal: normals) {
        debugHitboxes.push_back(normal);
    }

    std::vector<sf::RectangleShape> points = sat.returnPoints(enemyShipObj->shipSprite);
    for (auto& point: points) {
        debugHitboxes.push_back(point);
    }

}

void Game::renderEnemy() {
    for (int i = 0; i < enemyShips.size(); i++) {
        enemyShips.at(i)->render(this->window);
    }
}

void Game::renderProjectiles() {
    for (int i = 0; i < projectilesList.size(); i++) {
        projectilesList.at(i)->render(this->window);
        //this function, render(), belongs to the Projectile class.
        moveProjectiles(projectilesList.at(i), i);
        //moveProjectiles() should eventually be moved to the update loop.
    }
}

void Game::moveProjectiles(Projectile* projectile, int i) {
    sf::Vector2f elapsedDistance = projectile->projectileSprite.getPosition() - projectile->spawnedAt;
    //if the projectile has travelled more than 400 units, delete it.
    float distanceLength = std::sqrt(elapsedDistance.x * elapsedDistance.x + elapsedDistance.y * elapsedDistance.y);
    if (distanceLength > 900) {
        //projectile will begin to fade out after 900 units, rather than just disappear at 1000.
        projectile->projectileSprite.setColor(sf::Color(255,255,255,900-distanceLength));
    }
    if (distanceLength > 1000) {
        projectilesList.erase(projectilesList.begin() + i);
        delete projectile;
        projectile = nullptr;
    } else {

        sf::Vector2f goTo;
        //A vector with magnitude one pointing in the direction of the projectile. 
        //This vector is created by subtracting the vector of the projectile's spawn point
        //from where the player clicked, stored as the directionOfTravel vector inside the Projectile object.
        goTo = projectile->directionOfTravel - projectile->spawnedAt;

        float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
        projectile->projectileSprite.setRotation(rotation);

        float length = std::sqrt(goTo.x * goTo.x + goTo.y * goTo.y);
        goTo = goTo / length;

        projectile->projectileSprite.move(goTo * projectile->speed * deltaTime);

    }
}

void Game::checkCollisions() {
    for (Ship* ship : allShips) {
        for (int i = 0; Projectile* projectile : projectilesList) {
            //compares the intersection of the sprite's bounding rectangle. 
            //This should be extended to check the coordinates of the projectile, and see if it
            // intersects the coordinates of a ship's system or room.
            sf::FloatRect projectileBounds = projectile->getSprite().getGlobalBounds();
            sf::FloatRect shipBounds = ship->getBoundingBox();
            if (projectileBounds.intersects(shipBounds)) {
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    // if both the projectile and the ship do not have the same value for their friendly boolean, collision will be registered.
                    if(satHelper.checkCollision(ship->shipSprite, projectile->projectileSprite)) {
                        // if contact has been made, now we can check if the projectile hit any systems.
                        // hull damage. Shields should be applied eventually.
                        ship->totalCondition -= projectile->damage;
                        logEvent("Ship has taken damage.");

                        //iterate over the systems map in the ship that is hit, and see if the projectile has hit any systems.
                        //Despite writing all that stuff for separating axis theorem, point in polygon may be better for the systems, since there are many of them!
                        for (auto& pair : ship->shipSystems) {
                            System& system = pair.second;
                            system.setHitbox(ship);
                        }
                        // use the iterator to erase the projectile from list, then delete.
                        projectilesList.erase(projectilesList.begin() + i);
                        if (projectile != nullptr) {
                            delete projectile;
                            projectile = nullptr; // Set the pointer to null after deletion
                            break;
                        }     
                    }  
                }
            }
            //check for whether ship is destroyed or not after all is done.
            ship->checkDamage();
            i++;
        }
    }
}

// will be run each frame. will eventually need code to check the type of weapon.
void Game::fireWeapon(Ship& firingShip) {
    //need to constantly update the sprite object in the Ship object. That's annoying.

    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().height / 2});
    sf::Vector2f directionOfTravel = (sf::Vector2f)sf::Mouse::getPosition(*window);
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && weaponSelected){
        Projectile* torpedo = new Projectile("../resource/photontorpedo.png", parentTip.x, parentTip.y,
                                            directionOfTravel, 1000.0, 0.1);      
        torpedo->setFriendly(); // the player is the only one to use this function, so it will be a friendly projectile.                            
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
        sf::Vector2f movementDistance = (sf::Vector2f) mousePosition - playerShipObj.shipSprite.getPosition();
        float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
        
        if (length != 0.0f) {
            sf::Vector2f normalizedVector = movementDistance / length;

            float speed = 500.0f;
            //ship's impulse speed stat multiplied to give effective speed
            float pSpeed = playerShipObj.impulseSpeed * speed;
            playerShipObj.shipSprite.move(normalizedVector * pSpeed * deltaTime);


            /*Find the angle of the distance vector using atan2, and convert to degrees. Then normalize it to be from 0 to 360 degrees. */
            float distanceAngle = (180.0f / M_PI * atan2(normalizedVector.y, normalizedVector.x));
            distanceAngle = std::fmod(distanceAngle + 360.0f, 360.0f);
            //Algorithm for determining if it is closer to rotate clockwise or counterclockwise to the target angle. 
            //Will not trigger if the ship's orientation is within 10 degrees of where the user is currently clicking.
            int cwDistance;
            int ccwDistance;
            if (abs(playerShipObj.shipSprite.getRotation() - distanceAngle) > 10) {
                if (distanceAngle >= playerShipObj.shipSprite.getRotation()) {
                    cwDistance = distanceAngle - playerShipObj.shipSprite.getRotation();
                    ccwDistance = playerShipObj.shipSprite.getRotation() + 360.0f - distanceAngle;
                } else {
                    cwDistance = 360.0f - playerShipObj.shipSprite.getRotation() + distanceAngle;
                    ccwDistance = playerShipObj.shipSprite.getRotation() - distanceAngle;
                }

                if (ccwDistance > cwDistance) {
                    playerShipObj.shipSprite.rotate(300 * deltaTime);
                } else if (ccwDistance < cwDistance) {
                    playerShipObj.shipSprite.rotate(-300 * deltaTime);
                }
            }
            
        }
        
    }
}

Game::Game() {
    this->initVariables();
    this->initWindow();
    this->initPlayer();
    this->initEnemy();
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
    this->checkCollisions();
    
    
}

void Game::render() {
    renderProjectiles();
    renderPlayer();
    renderEnemy();
    displayEvents();
    if (debugMode)
        renderDebugObjects();
    this->window->display();
}

void Game::logEvent(std::string event) {
    if (eventLog.size() >= 5) {
        eventLog.pop_back();
    }
    eventLog.push_back(event);
}

//create the text at the bottom of the log, and move up by reducing the offset down the screen.
void Game::displayEvents() {
    int positionOffset = 5;
    for (std::string event: eventLog) {
        sf::Text text(event, font);
        //text will become more transparent as it moves up the log.
        text.setColor(sf::Color(255, 255, 255, 51 * positionOffset));
        text.setPosition(0, 25 * positionOffset);
        positionOffset--;
        window->draw(text);
    }
}