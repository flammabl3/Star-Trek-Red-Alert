#include "Game.hpp"

#include <iostream>
#include <cmath>
#include <windows.h>


#include "ship.hpp"
#include "NCC-1701-D.hpp"
#include "Projectile.hpp"
#include "random0_n.hpp"

float rotation = 0;

//TODO: See if we can't come up with a shorter way to reference the playerShipObj's shipSprite member
// Hull damage has been somewhat accounted for with totalCondition
// Add shields and shielding system
// Improve functions to check for and apply damage, and damage rooms, subsystems, and personnel
// Create some kind of enemy AI.
// Add systems and system performance, and performance of systems based on crew and rooms

/*General path to completion
Systems and damage -> more weapons -> Ship AI -> UI -> sounds and animations -> rest of the game

Our conundrum: Some systems are "inside" others given the top down view. Some systems cannot be hit from the outside of the ship's hitbox.
Solutions:
Let the projectile hit the ship, but don't delete it till hit hits a system.
Hit the ship but let it hit several things, having a chance to be deleted each frame. (NOT optimal for torpedoes).
Let the player target individual systems. (Like FTL.)
Keep the current system and let beam weapons have multihit capability.

Weapon fire rate should be tied to the respective weapons system.
Speed and maneuverability should be tied to nacelles/engines.
Response time to user input should be tied to the bridge.
The bridge and power from engineering should be tied to everything. Reduced bridge/warp core should 
affect all systems.

Dead crewmembers should be replaced by live ones in the same room not assigned to a station.
Rank checks should take place (an ensign should not take the captain's chair unless there are no other options)

Fire and Oxygen should affect rooms.
*/

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
    playerShipPointer = &playerShipObj;
    allShips.push_back(playerShipPointer);
}

void Game::updatePlayer() {
    this->movePlayer();
}

void Game::renderPlayer() {
    playerShipObj.render(this->window);
}

void Game::showRoomDamageEnemy() {
    int positionOffset = 10;
    for (Ship* ship: enemyShips) {
        for (auto& pair: ship->shipSystems) {
            System& system = pair.second;
            for (Room& room: system.rooms) {
                std::string fireSize;
                if (room.fire == 0)
                    fireSize = "no fire";
                else if (25 > room.fire > 0) 
                    fireSize = "small fire";
                else if (room.fire > 25) 
                    fireSize = "medium fire";
                else if (room.fire > 50)
                    fireSize = "large fire";
                else if (room.fire > 75)
                    fireSize = "huge fire";   
                else if (room.fire > 100) 
                    fireSize = "massive fire";
                    
                std::string roomStats = room.roomType + ": " + fireSize + " " + std::to_string((int)room.oxygen) + "% oxygen " + std::to_string((int)system.power) + "% power";
                sf::Text text(roomStats, font);
                text.setScale(0.5, 0.5);
                //text will become more transparent as it moves up the log.
                text.setColor(sf::Color(255, 255 - room.fire, 255 - room.fire, 255));
                text.setPosition(0, 400 + 12 * positionOffset);
                positionOffset--;
                window->draw(text);
            }
        }
        
    }
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
    enemyShipObj->shipSprite.setRotation(110);
    enemyShipObj->friendly = false;

}

void Game::createDebugBoxes(Ship* enemyShipObj) {
    SATHelper sat;

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

void Game::updateEnemy() {
    enemyShips.at(0)->shipSprite.setRotation(rotation);
    rotation+=0.01;

    debugHitboxes.clear();
    createDebugBoxes(enemyShips.at(0));

    //The player's bounding box appears in the wrong spot.
    //createDebugBoxes(playerShipPointer);
}

void Game::updateAllShips() {
    //This should be split to log enemy and player events separately.
    for (Ship* ship: allShips) {
        std::vector<std::string> outputLog = ship->checkDamage();
        for (std::string string: outputLog) {
            logEvent(string);
        }
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
    int projectileDamage = 0;
    std::vector<Projectile*> toRemove;

    for (Ship* ship : allShips) {
        for (auto it = projectilesList.begin(); it != projectilesList.end(); ) {
            Projectile* projectile = *it;

            sf::FloatRect projectileBounds = projectile->getSprite().getGlobalBounds();
            sf::FloatRect shipBounds = ship->getBoundingBox();
            if (projectileBounds.intersects(shipBounds)) {
                
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    if (satHelper.checkCollision(ship->shipSprite, projectile->projectileSprite)) {
                        projectileDamage = projectile->damage - ship->shields/5;
                        
                        if (ship->shields > 0) {
                            ship->shields -= projectile->damage;
                            logEvent("Shields at " + std::to_string(ship->shields) + " percent.");
                        } 

                        if (projectileDamage > 0) {
                            ship->totalCondition -= projectileDamage;
                            logEvent("Ship has taken damage.");
                        }

                        std::map<std::string, System>::iterator randomSystem = ship->shipSystems.begin();
                        std::advance(randomSystem, random0_n(ship->shipSystems.size()));

                        std::vector<Room>::iterator randomRoom = randomSystem->second.rooms.begin(); 
                        std::advance(randomRoom, random0_n(randomSystem->second.rooms.size()));

                        //projectile damage should be modulated by the shields.
                        std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectileDamage);
                        for (std::string personnelLogged: damagedPersonnel) {
                            logEvent(personnelLogged);
                        }
                        ship->totalCondition -= projectile->damage;

                        for (auto& pair : ship->shipSystems) {
                            pair.second.setHitbox(ship);
                            //check for collision and log the string returned by checkCollision
                            std::string systemLogged = pair.second.checkCollision(projectile);
                            if (systemLogged.size() > 0)
                                logEvent(systemLogged);
                                pair.second.dealDamageToSystem(projectileDamage);
                        }
                        
                        // Log before erasing
                        it = projectilesList.erase(it);
                        delete projectile;
                        continue; // Move to the next iteration
                    }
                }
            }
            //should this only be called once?
            //ship->checkDamage();
            ++it;
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
                                            directionOfTravel, 1000.0, 10);      
        torpedo->setFriendly(); // the player is the only one to use this function, so it will be a friendly projectile.                            
        this->projectilesList.insert(projectilesList.end(), torpedo);
        weaponSelected = false;
    }
}

void Game::movePlayer() {
    // these variables, and this function itself should eventually be moved to the Ship class.
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        /* Get the position of the player's ship and the position of the mouse as vectors. 
        Find the vector which is the difference between the 2 vectors and normalize it by dividing by length. The vector is normalized so it can be multiplied by a constant speed.
        Move by the difference vector times speed times deltatime. */
        mousePosition = sf::Mouse::getPosition(*window);
        sf::Vector2f movementDistance = (sf::Vector2f) mousePosition - playerShipObj.shipSprite.getPosition();
        float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
        
        if (length != 0.0f) {
            sf::Vector2f normalizedVector = movementDistance / length;

            float speed = 100.0f;
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
                    playerShipObj.shipSprite.rotate(70 * deltaTime);
                } else if (ccwDistance < cwDistance) {
                    playerShipObj.shipSprite.rotate(-70 * deltaTime);
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

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (weaponSelected) {
                fireWeapon(playerShipObj);
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
            
        }
    }
}

void Game::update() {
    deltaTime = clock.restart().asSeconds();
    this->window->clear();
    this->updateEvents();
    this->updatePlayer();
    this->updateEnemy();
    this->checkCollisions();
    this->updateAllShips();
    
}

void Game::render() {
    renderProjectiles();
    renderPlayer();
    renderEnemy();
    displayEvents();
    showRoomDamageEnemy();

    if (debugMode)
        renderDebugObjects();
    this->window->display();
}

void Game::logEvent(std::string event) {
    if (eventLog.size() >= 10) {
        eventLog.pop_back();
    }
    eventLog.insert(eventLog.begin(), event);
}

//create the text at the bottom of the log, and move up by reducing the offset down the screen.
void Game::displayEvents() {
    int positionOffset = 10;
    for (std::string event: eventLog) {
        sf::Text text(event, font);
        text.setScale(0.5, 0.5);
        //text will become more transparent as it moves up the log.
        text.setColor(sf::Color(255, 255, 255, 25 * positionOffset));
        text.setPosition(0, 12 * positionOffset);
        positionOffset--;
        window->draw(text);
    }
}
