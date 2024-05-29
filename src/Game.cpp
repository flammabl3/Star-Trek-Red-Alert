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
// Improve functions to check for and apply damage, and damage rooms, subsystems, and personnel
// Create some kind of enemy AI.
// Add systems and system performance, and performance of systems based on crew and rooms
// ALL MOVEMENT MUST BE DELTATIMED.

/*General path to completion
Systems and damage -> more weapons -> Ship AI -> UI -> sounds and animations -> rest of the game

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
    this->weaponSelected = 0;
    timer.getElapsedTime();
    torpedoTime = 0;
    mov = 1;

    //pick a font later.
    font.loadFromFile("../resource/arial.ttf");

    debugMode = false;
}

void Game::initWindow() {
    this->videoMode.width = 1366;
    this->videoMode.height = 768;
    this->window = new sf::RenderWindow(sf::VideoMode(1366, 600), "Star Trek: Red Alert");
    this->window->setKeyRepeatEnabled(false);

    sf::Vector2u size = this->window->getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    view = sf::View(sf::FloatRect(0.f, 0.f, 1366.f, 768.f));
    //view.zoom(1.0f);

}

void Game::initPlayer() {
    playerShipObj = getEnterprise(); // The Ship object associated with the player's ship will be made the USS enterprise using a function from NCC-1701-D.hpp.
    playerShipObj.setSFMLObjects("../resource/Ent-D.png"); // Call function to set texture and sprite.
    playerShipObj.shipSprite.setPosition(700, 500);
    playerShipObj.shipSprite.setRotation(270);
    playerShipObj.setFriendly();
    //create a pointer to reference our player ship object, and add it to the vector. This seems... non-optimal.
    playerShipPointer = &playerShipObj;
    allShips.push_back(playerShipPointer);
    //use a member variable not a parameter for shield size.
    playerShipObj.setShield(300);
}

void Game::updatePlayer() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        movePlayer();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (weaponSelected == 1) {
            fireTorpedo(playerShipObj);
            weaponSelected = 0;
        }
        if (weaponSelected == 2) {
            //double shot
            fireDisruptor(playerShipObj); 
            weaponSelected = 0;
        }
        if (weaponSelected == 3) {
            firePhaser(playerShipObj);
            weaponSelected = 0;
        }
        if (weaponSelected == 4) {
            fireTorpedoSpread(playerShipObj); 
            weaponSelected = 0;
        }
    }
    playerShipObj.shieldOpacMod();
}

void Game::renderPlayer() {
    playerShipObj.render(this->window);
    this->window->draw(playerShipObj.shieldSprite);
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

void Game::renderEnemyHitboxes() {
    for (sf::RectangleShape rectangle: enemyHitboxes) {
        sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
        if (checkCollisionRectangleShape(rectangle, mousePosition)) {
            this->window->draw(rectangle);
        }
    }
}

//placeholder code will generate another USS enterprise for shooting at.
void Game::initEnemy() {
    SATHelper sat; //using the same SATHelper multiple times throughout the code causes errors.
    Ship* enemyShipObj = getEnterprisePointer();
    enemyShipObj->setSFMLObjects("../resource/Ent-D.png");
    enemyShipObj->shipSprite.setPosition(300, 100);
    enemyShipObj->shipSprite.setRotation(110);
    enemyShipObj->friendly = false;
    enemyShipObj->setShield(300);
    enemyShips.push_back(enemyShipObj);
    allShips.push_back(enemyShipObj);
}

void Game::createEnemyHitboxes(Ship* enemyShipObj) {
    for (auto& pair : enemyShipObj->shipSystems) {
        System& system = pair.second;
        system.setHitbox(enemyShipObj);
        enemyHitboxes.push_back(system.returnHitbox());
    }
}

void Game::createDebugBoxes(Ship* enemyShipObj) {
    SATHelper sat;

    debugHitboxes.push_back(enemyShipObj->returnHitbox());
    debugHitboxes.push_back(enemyShipObj->shieldRect);

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
    for (int i = 0; i < projectilesList.size(); i++) {
        if (dynamic_cast<Phaser*>(projectilesList.at(i)) != nullptr) {
            Phaser* p = dynamic_cast<Phaser*>(projectilesList.at(i));
            debugHitboxes.push_back(p->phaserRect);
        }
    }
}

void Game::renderEnemy() {
    for (int i = 0; i < enemyShips.size(); i++) {
        enemyShips.at(i)->render(this->window);
        this->window->draw(enemyShips.at(i)->shieldSprite);
    }
}

void Game::updateEnemy() { //extend this later
    enemyShips.at(0)->shipSprite.setRotation(rotation);

    enemyShips.at(0)->shieldOpacMod();

    rotation += 50 * deltaTime;

    if (enemyShips.at(0)->shipSprite.getPosition().x > 1000) {
        mov = -1;
    } else if (enemyShips.at(0)->shipSprite.getPosition().x < 0) {
        mov = 1;
    }
    
    enemyShips.at(0)->shipSprite.move(sf::Vector2f(mov, 0) * 100.0f * deltaTime);

    debugHitboxes.clear();
    createDebugBoxes(enemyShips.at(0));
    enemyHitboxes.clear();
    createEnemyHitboxes(enemyShips.at(0));

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
        
        //don't render if the disruptor shot has the secondShot condition, which will hide the projectile until it is time to fire.
        if (dynamic_cast<Disruptor*>(projectilesList.at(i)) == nullptr || !dynamic_cast<Disruptor*>(projectilesList.at(i))->secondShot)
            projectilesList.at(i)->render(this->window);
            //this function, render(), belongs to the Projectile class.

        //if the dynamic cast does not return nullptr, that means it is that type.
        //necessary because pushing a subclass to a vector of base class results in an implicit upcast
        if (dynamic_cast<Torpedo*>(projectilesList.at(i)) != nullptr) {
            moveTorpedoes(dynamic_cast<Torpedo*>(projectilesList.at(i)), i);
        }
        else if (dynamic_cast<Disruptor*>(projectilesList.at(i)) != nullptr)
            moveDisruptors(dynamic_cast<Disruptor*>(projectilesList.at(i)), i);

        else if (dynamic_cast<Phaser*>(projectilesList.at(i)) != nullptr)
            movePhasers(dynamic_cast<Phaser*>(projectilesList.at(i)), i);
    }
}

void Game::moveTorpedoes(Torpedo* projectile, int i) {
    torpedoTime += deltaTime;
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
    } else if (projectile != nullptr) {
        sf::Vector2f goTo;
        //A vector with magnitude one pointing in the direction of the projectile. 
        //This vector is created by subtracting the vector of the projectile's spawn point
        //from where the player clicked, stored as the directionOfTravel vector inside the Projectile object.

        //this is the default value, indicating that no value for a target was assigned.
        

        //linear interpolation
        //create a vector that represents the direction to the target. Normalize the vector where the projectile
        //starts pointing to and where it should end pointing to, then curve it to the target based on distance.
        //t is squared to cause more aggressive curving as the distance increases.
        if (projectile->targetingSystem) {
            //std::cout << projectile->targetPos.x << std::endl;
            projectile->targetPos = projectile->targetSystemObj->hitbox.getPosition();
            
            //only recalculate the target after given time interval, to avoid excessive updates
            if (torpedoTime > 0.001f || !projectile->hasPositionInitialized) {
                torpedoTime = 1;
                float t = std::min(1.0f, std::max(0.0f, distanceLength / 30));
                sf::Vector2f directionToTarget = projectile->targetPos - projectile->spawnedAt;

                float length0 = std::sqrt(projectile->directionOfTravel.x * projectile->directionOfTravel.x + projectile->directionOfTravel.y * projectile->directionOfTravel.y);
                float length1 = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);
                projectile->lastCalculatedPosition = (1 - t) * projectile->directionOfTravel / length0 + t * directionToTarget / length1;
                projectile->hasPositionInitialized = true;
            } 
            goTo = projectile->lastCalculatedPosition;
            
        } else {
            float t = std::min(1.0f, std::max(0.0f, distanceLength / 30.0f));
            sf::Vector2f directionToTarget = projectile->targetPos - projectile->spawnedAt;

            float length0 = std::sqrt(projectile->directionOfTravel.x * projectile->directionOfTravel.x + projectile->directionOfTravel.y * projectile->directionOfTravel.y);
            float length1 = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);
            goTo = (1 - t) * projectile->directionOfTravel / length0 + t * directionToTarget / length1;
        }
        

        float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
        projectile->projectileSprite.setRotation(rotation);

        float length = std::sqrt(goTo.x * goTo.x + goTo.y * goTo.y);
        goTo = goTo / length;

        projectile->projectileSprite.move(goTo * projectile->speed * deltaTime);
    }
}

void Game::moveDisruptors(Disruptor* projectile, int i) {
    if (!projectile->secondShot) {
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
        } else if (projectile != nullptr) {
            sf::Vector2f goTo;

            goTo = projectile->directionOfTravel - projectile->spawnedAt;

            float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
            projectile->projectileSprite.setRotation(rotation);

            float length = std::sqrt(goTo.x * goTo.x + goTo.y * goTo.y);
            goTo = goTo / length;

            projectile->projectileSprite.move(goTo * projectile->speed * deltaTime);
        }
    } else {
        if (projectile->secondShotDelay < 0.3) {
            projectile->secondShotDelay += deltaTime;
        } else {
            projectile->secondShot = false;

            //this should eventually be changed to be the tip of the disruptor which shot it.
            sf::Vector2f firingShipPos = projectile->firingShip->shipSprite.getPosition();
            projectile->spawnedAt = firingShipPos;
            projectile->projectileSprite.setPosition(firingShipPos);
        }
    }       
}

void Game::movePhasers(Phaser* projectile, int i) {
            sf::Vector2f goTo;
            goTo = projectile->directionOfTravel - projectile->spawnedAt;

            float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
            projectile->projectileSprite.setRotation(rotation);


            projectile->phaserTimer += deltaTime;
            if (projectile->phaserTimer > 0.01 && !projectile->hasCollided) {
                projectile->phaserScaleX += 5;
                projectile->projectileSprite.setScale(projectile->phaserScaleX, 0.25);
                projectile->phaserTimer = 0;
                projectile->phaserRect.setScale(projectile->phaserScaleX, 0.25);
                projectile->phaserRect.setPosition(projectile->projectileSprite.getPosition());
                projectile->phaserRect.setRotation(rotation);
            }
            if (projectile->phaserScaleX > 300) {
                projectilesList.erase(projectilesList.begin() + i);
                delete projectile;
                projectile = nullptr;
            }
}

void Game::checkCollisions() {
    bool hit = false;
    int projectileDamage = 0;
    std::vector<Projectile*> toRemove;

    for (Ship* ship : allShips) {
        if (hit)
            break;
        for (auto it = projectilesList.begin(); it != projectilesList.end(); ) {
            if (hit)
                break;
            Projectile* projectile = *it;
            sf::FloatRect projectileBounds = projectile->getSprite().getGlobalBounds();
            sf::FloatRect shipBounds = ship->getBoundingBox();
            //shipBounds should be replaced by a square shield bubble hitbox.
            if (checkCollisionRectangleShape(ship->shieldRect, projectile->projectileSprite.getPosition())) {
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    if (ship->shields > 0) {
                        ship->shields -= projectile->damage;
                        ship->shieldHit(projectile->projectileSprite.getPosition());
                        if (ship->shields < 0) {
                            ship->shields = 0;
                        }
                        logEvent("Shields at " + std::to_string(ship->shields) + " percent.");
                        it = projectilesList.erase(it);
                        delete projectile;
                        continue;
                    }
                }
            }

            if (projectileBounds.intersects(shipBounds)) {
                projectileDamage = projectile->damage - ship->shields/5;
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    if (projectile->touchingTarget || satHelper.checkCollision(ship->shipSprite, projectile->projectileSprite)) {
                        projectile->touchingTarget = true;
                        if (Torpedo* torpedo = dynamic_cast<Torpedo*>(projectile); torpedo != nullptr) {
                            if (!torpedo->missed && torpedo->targetingSystem) {
                                sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
                                for (auto& pair: ship->shipSystems) {
                                    if (hit || torpedo->missed)
                                        break;
                                    System system = pair.second; 
                                    if (system.systemType == torpedo->targetSystem) {
                                        if (system.checkCollision(projectile->projectileSprite.getPosition())) {
                                            if (random0_n(100) <= torpedo->hitChance) {
                                                std::cout << "SYSTEM HIT" << std::endl;
                                                logEvent(system.dealDamageToSystem(torpedo->damage));
                                                it = projectilesList.erase(it);
                                                delete projectile;
                                                hit = true;
                                            } else {
                                                std::cout << "MISS" << std::endl;
                                                projectile->missed = true;
                                                miniTextCreate("MISS", projectile->projectileSprite.getPosition());
                                            }
                                        }
                                    }
                                }
                            } 
                            
                            if (!torpedo->targetingSystem && !torpedo->missed) {
                                if (random0_n(100) <= torpedo->hitChance) {
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
                                    ship->totalCondition -= torpedo->damage;

                                    for (auto& pair : ship->shipSystems) {
                                        pair.second.setHitbox(ship);
                                        //check for collision and log the string returned by checkCollision

                                        //if a string was returned there was a collision
                                        if (pair.second.checkCollision(torpedo->projectileSprite.getPosition()))
                                            logEvent(pair.second.dealDamageToSystem(projectileDamage));
                                    }
                                    
                                    // Log before erasing
                                    
                                    
                                    it = projectilesList.erase(it);
                                    delete torpedo;

                                    
                                    continue; // Move to the next iteration
                                } else {
                                    std::cout << "MISS" << std::endl;
                                    torpedo->missed = true;
                                    miniTextCreate("MISS", projectile->projectileSprite.getPosition());
                                }
                            }
                        } else if (Disruptor* disruptor = dynamic_cast<Disruptor*>(projectile); disruptor != nullptr) {
                            if (satHelper.checkCollision(ship->shipSprite, disruptor->projectileSprite)) {
                                if (random0_n(100) <= disruptor->hitChance) {
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
                                    ship->totalCondition -= disruptor->damage;

                                    for (auto& pair : ship->shipSystems) {
                                        pair.second.setHitbox(ship);
                                        //check for collision and log the string returned by checkCollision

                                        //if a string was returned there was a collision
                                        if (pair.second.checkCollision(disruptor->projectileSprite.getPosition()))
                                            logEvent(pair.second.dealDamageToSystem(projectileDamage));
                                    }
                                    
                                    // Log before erasing
                                    
                                    
                                    it = projectilesList.erase(it);
                                    delete disruptor;

                                    continue; // Move to the next iteration
                                } else {
                                    disruptor->missed = true;
                                    std::cout << "MISS" << std::endl;
                                    miniTextCreate("MISS", projectile->projectileSprite.getPosition());
                                }
                            }
                        }
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
void Game::fireTorpedo(Ship& firingShip) {
    //need to constantly update the sprite object in the Ship object. 
    
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().height / 2});
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
    float cosValue = cos(firingShip.shipSprite.getRotation() * M_PI / 180);
    float sinValue = sin(firingShip.shipSprite.getRotation() * M_PI / 180);
    
    if (abs(cosValue) < 0.0001) {
        cosValue = 0;
    }
    if (abs(sinValue) < 0.0001) {
        sinValue = 0;
    }
    sf::Vector2f directionOfTravel = sf::Vector2f(cosValue, sinValue);
    
    Torpedo* torpedo = new Torpedo("../resource/photontorpedo.png", parentTip.x, parentTip.y,
                                        directionOfTravel, 1000.0, 10);
    for (Ship* ship: enemyShips) {
        sf::FloatRect shipBounds = ship->shipSprite.getGlobalBounds();
        if (shipBounds.contains(mousePosition)) {
            for (auto& pair: ship->shipSystems) {
                System& system = pair.second; 
                if (system.checkCollision(mousePosition)) {
                    torpedo->targetSystem = system.systemType;
                    torpedo->targetSystemObj = &system;
                    torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                    torpedo->targetingSystem = true;
                    break;
                }

                
            }
            if (torpedo->targetingSystem == false) {
                // if the ship is clicked but not a system, just target a random system.
                std::map<std::string, System>::iterator randomSystem = ship->shipSystems.begin();
                std::advance(randomSystem, random0_n(ship->shipSystems.size()));
                if (randomSystem != ship->shipSystems.end()) {
                    System& system = randomSystem->second;
                    torpedo->targetSystem = system.systemType;
                    torpedo->targetSystemObj = &system;
                    torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                    torpedo->targetingSystem = true;
                }
                
            }
        } else {
            torpedo->hitChance = 20;
            torpedo->targetPos = mousePosition;
        }
    }   

    if (firingShip.friendly)
        torpedo->setFriendly(); 
    this->projectilesList.insert(projectilesList.end(), torpedo);

}

void Game::fireTorpedoSpread(Ship& firingShip) {

    //need to constantly update the sprite object in the Ship object. 
    
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().height / 2});
    
    float cosValue = cos(firingShip.shipSprite.getRotation() * M_PI / 180);
    float sinValue = sin(firingShip.shipSprite.getRotation() * M_PI / 180);
    
    if (abs(cosValue) < 0.0001) {
        cosValue = 0;
    }
    if (abs(sinValue) < 0.0001) {
        sinValue = 0;
    }

    for (int i = 0; i < 5; i++) {
        sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
        sf::Vector2f directionOfTravel = sf::Vector2f(cosValue, sinValue);
        
        Torpedo* torpedo = new Torpedo("../resource/photontorpedo.png", parentTip.x, parentTip.y,
                                            directionOfTravel, 1000.0, 10);
        for (Ship* ship: enemyShips) {
            sf::FloatRect shipBounds = ship->shipSprite.getGlobalBounds();
            if (shipBounds.contains(mousePosition)) {
                // if the ship is clicked but not a system, just target a random system.
                std::map<std::string, System>::iterator randomSystem = ship->shipSystems.begin();
                std::advance(randomSystem, random0_n(ship->shipSystems.size()));
                if (randomSystem != ship->shipSystems.end()) {
                    System& system = randomSystem->second;
                    torpedo->targetSystem = system.systemType;
                    torpedo->targetSystemObj = &system;
                    torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                    torpedo->targetingSystem = true;
                }
            } else {
                mousePosition += sf::Vector2f(randomNegPos() * random0_n(150), randomNegPos() * random0_n(150));
                torpedo->targetPos = mousePosition;
            }
        }   

        torpedo->damage = 2;
        torpedo->hitChance = 50;
        if (firingShip.friendly)
            torpedo->setFriendly(); 
        this->projectilesList.insert(projectilesList.end(), torpedo);
    }
}

void Game::fireDisruptor(Ship& firingShip) {
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().height / 2});
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
    
    Disruptor* disruptor = new Disruptor("../resource/disruptor.png", parentTip.x, parentTip.y,
                                        mousePosition, 1700.0, 6);  

    Disruptor* disruptor2 = new Disruptor("../resource/disruptor.png", parentTip.x, parentTip.y,
                                        mousePosition, 1700.0, 6);  
    disruptor2->secondShot = true;
    disruptor2->firingShip = &firingShip;

    if (firingShip.friendly) {
        disruptor->setFriendly();
        disruptor2->setFriendly();
    }

    this->projectilesList.insert(projectilesList.end(), disruptor);
    this->projectilesList.insert(projectilesList.end(), disruptor2);
}

void Game::movePlayer() {
    // these variables, and this function itself should eventually be moved to the Ship class.
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

void Game::firePhaser(Ship& firingShip) {
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().height / 2});
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
    
    Phaser* phaser = new Phaser("../resource/phaser.png", parentTip.x, parentTip.y,
                                        mousePosition, 1700.0, 6);  

    if (firingShip.friendly) {
        phaser->setFriendly();
    }
    phaser->damage = 1;

    this->projectilesList.insert(projectilesList.end(), phaser);
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
    while (this->window->pollEvent(event)) {
        if (this->event.type == sf::Event::Closed) {
            this->window->close();
            exit(0);
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.scancode == sf::Keyboard::Scan::Num1)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                weaponSelected = 1;
                std::cout << "weapon 1 selected" << std::endl;
            }

            if (event.key.scancode == sf::Keyboard::Scan::Num2)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                weaponSelected = 2;
                std::cout << "weapon 2 selected" << std::endl;
            }

            if (event.key.scancode == sf::Keyboard::Scan::Num3)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                weaponSelected = 3;
                std::cout << "weapon 3 selected" << std::endl;
            }

            if (event.key.scancode == sf::Keyboard::Scan::Num4)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                weaponSelected = 4;
                std::cout << "weapon 4 selected" << std::endl;
            }

            if (event.key.scancode == sf::Keyboard::Scan::Num0)
            {
                // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                if (debugMode) {
                    debugMode = false; 
                    logEvent("Debug mode off.");
                } else {
                    debugMode = true;
                    logEvent("Debug mode on.");
                }
                    
            }
        }
    }
}

void Game::update() {
    deltaTime = clock.restart().asSeconds();

    //use this to fix the homing weapons issue.
    if (timer.getElapsedTime().asSeconds() > 0.00100001)
        timer.restart();
    timer.getElapsedTime();

    this->window->clear();
    this->updateEvents();
    this->updatePlayer();
    this->updateEnemy();
    this->updateAllShips();
    this->checkCollisions();
}

void Game::render() {
    renderProjectiles();
    renderPlayer();
    renderEnemy();
    displayEvents();
    displayMiniText();
    showRoomDamageEnemy();
    renderEnemyHitboxes();

    if (debugMode)
        renderDebugObjects();
    this->window->display();
    //this->window->setView(view);
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

void Game::miniTextCreate(std::string text, sf::Vector2f pos) {
    sf::Text miniText(text, font);
    miniText.setScale(0.5, 0.5);
    //text will become more transparent as it moves up the log.
    miniText.setColor(sf::Color(255, 255, 255, 255));
    miniText.setPosition(pos);
    miniTextVect.push_back(std::tuple(miniText, 255));
}

void Game::displayMiniText() {
    std::vector<std::tuple<sf::Text, int>>::iterator it;
    for (it = miniTextVect.begin(); it < miniTextVect.end(); it++) {
        auto& tuple = *it;
        if (std::get<1>(tuple) > 0) {
            std::get<0>(tuple).setColor(sf::Color(255, 255, 255, std::get<1>(tuple)));
            std::get<1>(tuple)--;
            window->draw(std::get<0>(tuple));
        } else {
            miniTextVect.erase(it);
        }
    }
    
}

bool Game::checkCollisionRectangleShape(sf::RectangleShape rect, sf::Vector2f vect) {
    sf::Vector2f translatedVect = vect - rect.getPosition();
    //move vector to origin of rectangle.

    // Rotate point in opposite direction of rectangle
    float theta = -rect.getRotation() * M_PI / 180.0f;
    sf::Vector2f rotatedVect;
    rotatedVect.x = translatedVect.x * cos(theta) - translatedVect.y * sin(theta);
    rotatedVect.y = translatedVect.x * sin(theta) + translatedVect.y * cos(theta);

    // Check if rotated point is inside axis-aligned rectangle
    float hw = rect.getSize().x / 2.0f; // half width
    float hh = rect.getSize().y / 2.0f; // half height
    if ((-hw <= rotatedVect.x && rotatedVect.x <= hw) && (-hh <= rotatedVect.y && rotatedVect.y <= hh)) {
        return true;
    }
    return false;
}