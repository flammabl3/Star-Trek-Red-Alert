#include "Game.hpp"

#include <iostream>
#include <cmath>
#include <windows.h>


#include "Ship.hpp"
#include "NCC-1701-D.hpp"
#include "Projectile.hpp"
#include "random0_n.hpp"

float rot = 0;

//TODO: See if we can't come up with a shorter way to reference the playerShipObj's shipSprite member
//SYSTEMS are mostly done, but capacity of personnel should be calculated based on health, skill, mental state!
// Improve functions to check for and apply damage, and damage rooms, subsystems, and personnel
// ALL MOVEMENT MUST BE DELTATIMED.

/*General path to completion
Systems and damage -> Ship AI -> change views and window sizes -> UI -> sounds and animations -> rest of the game

Response time to user input should be tied to the bridge.

Dead crewmembers should not replace others twice.
Rank checks should take place (an ensign should not take the captain's chair unless there are no other options)

*/

void Game::initVariables() {
    this->window = nullptr;
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
    playerShipObj = *getEnterprisePointer(); // The Ship object associated with the player's ship will be made the USS enterprise using a function from NCC-1701-D.hpp.
    playerShipObj.setSFMLObjects("../resource/Ent-D.png"); // Call function to set texture and sprite.
    playerShipObj.shipSprite.setPosition(700, 500);
    playerShipObj.shipSprite.setRotation(270);
    playerShipObj.setFriendly();
    //create a pointer to reference our player ship object, and add it to the vector. This seems... non-optimal.
    playerShipPointer = &playerShipObj;
    allShips.push_back(playerShipPointer);
    //use a member variable not a parameter for shield size.
    playerShipObj.setShield(300);

    for (auto& pair: playerShipObj.shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        system->parentShip = &playerShipObj;
    }

    for (auto& pair: playerShipPointer->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        system->parentShip = playerShipPointer;
    }
}

void Game::updatePlayer() {
    if (playerShipObj.totalCondition <= 0) {
        return;
    }
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        movePlayer();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        useWeapon(playerShipObj);
    }
    playerShipObj.shieldOpacMod();
}

void Game::renderPlayer() {
    if (playerShipObj.totalCondition > 0) {
        playerShipObj.render(this->window);
        this->window->draw(playerShipObj.shieldSprite);
    }
}

void Game::showRoomDamage() {
    int positionOffset = 10;
    for (auto& pair: playerShipPointer->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        for (Room& room: system->rooms) {
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
                
            std::string roomStats = room.roomType + ": " + fireSize + " " + std::to_string((int)room.oxygen) + "% oxygen " + std::to_string((int)system->power) + "% power";
            sf::Text text(roomStats, font);
            text.setScale(0.5, 0.5);
            //text will become more transparent as it moves up the log.
            text.setFillColor(sf::Color(255, 255 - room.fire, 255 - room.fire, 255));

            sf::Vector2i viewPosition = sf::Vector2i(0, 400 + 12 * positionOffset);
            text.setPosition(window->mapPixelToCoords(viewPosition));
            positionOffset--;
            window->draw(text);
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
    for (auto& pair: enemyShipObj->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        system->parentShip = enemyShipObj;
    }
    enemyShips.push_back(enemyShipObj);
    allShips.push_back(enemyShipObj);
}

void Game::createShipHitboxes(Ship* shipObj) {
    for (auto& pair : shipObj->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        system->setHitbox(shipObj);
        if (shipObj->friendly) {
            friendlyHitboxes.push_back(system->returnHitbox());
        } else {
            enemyHitboxes.push_back(system->returnHitbox());
        }
    }
}

void Game::createDebugBoxes(Ship* shipObj) {
    SATHelper sat;

    debugHitboxes.push_back(shipObj->returnHitbox());
    debugHitboxes.push_back(shipObj->shieldRect);

    for (auto& pair : shipObj->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        system->setHitbox(shipObj);
        debugHitboxes.push_back(system->returnHitbox());
    }

    std::vector<sf::RectangleShape> normals = sat.returnNormals(shipObj->shipSprite);
    for (auto& normal: normals) {
        debugHitboxes.push_back(normal);
    }

    std::vector<sf::RectangleShape> points = sat.returnPoints(shipObj->shipSprite);
    for (auto& point: points) {
        debugHitboxes.push_back(point);
    }

    for (int i = 0; i < projectilesList.size(); i++) {
        if (dynamic_cast<Phaser*>(projectilesList.at(i)) != nullptr) {
            Phaser* p = dynamic_cast<Phaser*>(projectilesList.at(i));
            debugHitboxes.push_back(p->phaserRect);
            std::vector<sf::RectangleShape> pts = sat.returnPoints(p->phaserRect);
            for (auto& point: pts) {
                debugHitboxes.push_back(point);
            }
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
    for (Ship* ship: enemyShips) {
        makeDecision(ship);
    }
}

void Game::updateAllShips() {
    std::vector<Ship*> destroyShips;

    for (auto it = allShips.begin(); it != allShips.end(); ++it) {
        Ship* ship = *it;
        for (auto& pair: ship->shipSystems) {
            //reference unique_ptr<System> stored in vector, Create a raw pointer reference to the raw pointer in the unique_ptr
            std::shared_ptr<System>& system = pair.second;
            Weapon* wep = dynamic_cast<Weapon*>(system.get());
            if (wep != nullptr && !wep->disabled) {
                wep->updateTimer(deltaTime);
            }
        }

        createDebugBoxes(ship);
        createShipHitboxes(ship);
        ship->shieldOpacMod();

        std::vector<std::string> outputLog = ship->checkDamage();
        for (std::string string: outputLog) {
            if (string == "DESTROYFLAG") {
                destroyShips.push_back(ship);
                break;
            } else {
                logEvent(string, ship->friendly);
            }
        }
    }

    for (auto ship : destroyShips) {
        allShips.erase(std::remove(allShips.begin(), allShips.end(), ship), allShips.end());
        enemyShips.erase(std::remove(enemyShips.begin(), enemyShips.end(), ship), enemyShips.end());
        friendlyShips.erase(std::remove(friendlyShips.begin(), friendlyShips.end(), ship), friendlyShips.end());


        if (ship != playerShipPointer)
            delete ship;
        else {
            playerShipPointer = nullptr;
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
                torpedoTime = 0;
                float t = std::min(1.0f, std::max(0.0f, distanceLength / 100.0f));
                //t = t*t*t;
                sf::Vector2f directionToTarget = projectile->targetPos - projectile->spawnedAt;
                //std::cout << t << std::endl;

                projectile->lastCalculatedPosition = (1 - t) * projectile->directionOfTravel + t * directionToTarget;
                projectile->hasPositionInitialized = true;

                //std::cout << projectile->lastCalculatedPosition.x << " " << projectile->lastCalculatedPosition.y << std::endl;
            } 
            goTo = projectile->lastCalculatedPosition;
            
        } else {
            float t = std::min(1.0f, std::max(0.0f, distanceLength / 30.0f));
            sf::Vector2f directionToTarget = projectile->targetPos - projectile->spawnedAt;

            goTo = (1 - t) * projectile->directionOfTravel + t * directionToTarget;
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
    //the phaser should start and stick to this point on the firing ship.
    //NOTE: in TNG, the phaser bank is a ring on top and on the bottom of the ship. Note that the phaser can appear anywhere on this ring.
    //This will get added later.
    projectile->firingShipOffset = projectile->firingShip->shipSprite.getTransform().transformPoint(
    projectile->firingShip->shipSprite.getLocalBounds().width - 4, projectile->firingShip->shipSprite.getLocalBounds().height / 2);

    projectile->projectileSprite.setPosition(projectile->firingShipOffset);

    if (projectile->targetShip != nullptr) {
        if (projectile->targetShip->shields > 0) {
            goTo = projectile->targetShip->shipSprite.getPosition() - projectile->targetShip->shieldOffset - projectile->firingShipOffset;;
            projectile->newTarget = goTo;
        } else {
            goTo = projectile->targetShip->shipSprite.getPosition() - projectile->firingShipOffset;;
            projectile->newTarget = goTo;
        }
    } else {
        goTo = projectile->directionOfTravel - projectile->firingShipOffset;;
        projectile->originalTarget = goTo;
    }
    float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
    projectile->projectileSprite.setRotation(rotation);
    projectile->phaserTimer += deltaTime;
    if (projectile->phaserTimer > 0.01 && !projectile->hasCollided) {
        projectile->phaserScaleX += 5;
        projectile->phaserTimer = 0;
    } else if (projectile->hasCollided) {
        // calculate the desired length of the projectile sprite when it touches the target
        float desiredLength = std::sqrt(projectile->newTarget.x * projectile->newTarget.x + projectile->newTarget.y * projectile->newTarget.y);
        
        // calculate the scaling factor based on the desired length and the original sprite length
        float originalLength = projectile->projectileSprite.getLocalBounds().width;
        projectile->phaserScaleX = desiredLength / originalLength;
    }

    //send phaserRect to the end of the phaser for collision.
    sf::Transform transform = projectile->projectileSprite.getTransform();
    sf::FloatRect bounds = projectile->projectileSprite.getLocalBounds();
    // local point at the end of the sprite
    sf::Vector2f localPoint(bounds.width, bounds.height / 2);
    // transform this point
    sf::Vector2f globalPoint = transform.transformPoint(localPoint);
    projectile->phaserRect.setPosition(globalPoint);
    projectile->phaserRect.setRotation(projectile->projectileSprite.getRotation());
    //subtract from scale so that the sprite actually touches the shield instead of slightly going through it
    projectile->projectileSprite.setScale(projectile->phaserScaleX - 1, 0.25);

    if (projectile->phaserScaleX > 300) {
        projectilesList.erase(projectilesList.begin() + i);
        delete projectile;
        projectile = nullptr;
    }
}

void Game::checkCollisions() {
    bool hit = false;

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
            int projectileDamage = projectile->damage;
            //check for shield collision
            if (checkCollisionRectangleShape(ship->shieldRect, projectile->projectileSprite.getPosition())) {
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    if (ship->shields > 0) {
                        ship->shields -= projectile->damage;
                        projectileDamage -= ship->shields/8;
                        ship->shieldHit(projectile->projectileSprite.getPosition(), true);
                        if (ship->shields < 0) {
                            ship->shields = 0;
                        }
                        if (projectileDamage > 0) {
                            std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                            std::advance(randomSystem, random0_n(ship->shipSystems.size()));
    
                            std::vector<Room>::iterator randomRoom = randomSystem->second->rooms.begin(); 
                            std::advance(randomRoom, random0_n(randomSystem->second->rooms.size()));

                            //projectile damage should be modulated by the shields.
                            std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectileDamage);
                            for (std::string personnelLogged: damagedPersonnel) {
                                logEvent(personnelLogged, ship->friendly);
                            } 
                        }
                        
                        logEvent("Shields at " + std::to_string(ship->shields) + " percent.", ship->friendly);
                        it = projectilesList.erase(it);
                        delete projectile;
                        continue;
                    }
                }
            }
            //special check for phaser collision
            if (Phaser* phaser = dynamic_cast<Phaser*>(projectile); phaser != nullptr) { 
                if (phaser->collidedDeleteTimer < 2.0 && phaser->hasCollided && phaser->targetShip == ship) {
                    if (ship->shields > 0) {
                        phaser->targetShip->shieldOpac = 255;
                        phaser->targetShip->shieldHit(phaser->phaserRect.getPosition(), false);
                    }
                    phaser->collidedDeleteTimer += deltaTime;
                } if (phaser->collidedDeleteTimer > 2.0) {
                    it = projectilesList.erase(it);
                    delete projectile;
                    continue;
                }
                //don't do any more checks if the phaser has collided, just deal damage once
                if (!phaser->hasCollided) {
                    if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                        if (satHelper.checkCollision(ship->shieldRect, phaser->phaserRect) && ship->shields > 0) {
                            phaser->hasCollided = true;
                            phaser->originalScale = phaser->phaserScaleX;
                            phaser->targetShip = ship;
                            ship->shields -= projectile->damage;
                            projectileDamage -= ship->shields/8;
                            ship->shieldHit(phaser->phaserRect.getPosition(), true);
                            if (ship->shields < 0) {
                                ship->shields = 0;
                            }
                            logEvent("Shields at " + std::to_string(ship->shields) + " percent.", ship->friendly);
                            if (phaser->collidedDeleteTimer > 3.0) {
                                it = projectilesList.erase(it);
                                delete projectile;
                                continue; 
                            }

                            if (projectileDamage > 0) {
                                std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                                std::advance(randomSystem, random0_n(ship->shipSystems.size()));
        
                                std::vector<Room>::iterator randomRoom = randomSystem->second->rooms.begin(); 
                                std::advance(randomRoom, random0_n(randomSystem->second->rooms.size()));

                                //projectile damage should be modulated by the shields.
                                std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectileDamage);
                                for (std::string personnelLogged: damagedPersonnel) {
                                    logEvent(personnelLogged, ship->friendly);
                                } 
                            }   
                        } else if (!phaser->missed && satHelper.checkCollision(ship->shipSprite, phaser->phaserRect)) {
                            if (random0_nInclusive(100) <= phaser->hitChance) {
                                phaser->hasCollided = true;
                                phaser->originalScale = phaser->phaserScaleX;
                                phaser->targetShip = ship;
                                
                                
                                ship->totalCondition -= projectile->damage;
                                logEvent("Ship has taken damage.", ship->friendly);
                                
                                std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                                std::advance(randomSystem, random0_n(ship->shipSystems.size()));

                                std::vector<Room>::iterator randomRoom = randomSystem->second->rooms.begin(); 
                                std::advance(randomRoom, random0_n(randomSystem->second->rooms.size()));

                                //projectile damage should be modulated by the shields.
                                std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectile->damage);
                                for (std::string personnelLogged: damagedPersonnel) {
                                    logEvent(personnelLogged, ship->friendly);
                                }

                                for (auto& pair : ship->shipSystems) {
                                    pair.second->setHitbox(ship);
                                    //check for collision and log the string returned by checkCollision

                                    //if a string was returned there was a collision
                                    if (pair.second->checkCollision(phaser->projectileSprite.getPosition()))
                                    logEvent(pair.second->dealDamageToSystem(projectile->damage), ship->friendly);
                                }
                                
                                // Log before erasing
                                if (phaser->collidedDeleteTimer > 3.0) {
                                    it = projectilesList.erase(it);
                                    delete projectile;
                                    continue; 
                                }
                            } else {
                                phaser->missed = true;
                                std::cout << "MISS" << std::endl;
                                miniTextCreate("MISS", phaser->phaserRect.getPosition());
                            }
                        }
                    }
                }
            }
            //check for disruptor/torpedo collision
            else if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                if (projectileBounds.intersects(shipBounds)) {
                    if (projectile->touchingTarget || satHelper.checkCollision(ship->shipSprite, projectile->projectileSprite)) {
                        projectile->touchingTarget = true;
                        if (Torpedo* torpedo = dynamic_cast<Torpedo*>(projectile); torpedo != nullptr) {
                            if (!torpedo->missed && torpedo->targetingSystem) {
                                sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(*window);
                                for (auto& pair: ship->shipSystems) {
                                    if (hit || torpedo->missed)
                                        break;
                                    std::shared_ptr<System>& system = pair.second; 
                                    if (system->systemType == torpedo->targetSystem) {
                                        if (system->checkCollision(projectile->projectileSprite.getPosition())) {
                                            if (random0_nInclusive(100) <= torpedo->hitChance) {
                                                std::cout << "SYSTEM HIT" << std::endl;
                                                
                                                ship->totalCondition -= projectile->damage;
                                                logEvent("Ship has taken damage.", ship->friendly);
                                            
                                                logEvent(system->dealDamageToSystem(projectile->damage), ship->friendly);
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
                                if (random0_nInclusive(100) <= torpedo->hitChance) {
                                    
                                    ship->totalCondition -= projectile->damage;
                                    logEvent("Ship has taken damage.", ship->friendly);
                                    
                                    
                                    std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                                    std::advance(randomSystem, random0_n(ship->shipSystems.size()));

                                    std::vector<Room>::iterator randomRoom = randomSystem->second->rooms.begin(); 
                                    std::advance(randomRoom, random0_n(randomSystem->second->rooms.size()));

                                    //projectile damage should be modulated by the shields.
                                    std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectile->damage);
                                    for (std::string personnelLogged: damagedPersonnel) {
                                        logEvent(personnelLogged, ship->friendly);
                                    }
                                    for (auto& pair : ship->shipSystems) {
                                        pair.second->setHitbox(ship);
                                        //check for collision and log the string returned by checkCollision

                                        //if a string was returned there was a collision
                                        if (pair.second->checkCollision(torpedo->projectileSprite.getPosition()))
                                            logEvent(pair.second->dealDamageToSystem(projectile->damage), ship->friendly);
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
                            if (!disruptor->missed) {
                                if (random0_nInclusive(100) <= disruptor->hitChance) {
                                    if (projectile->damage > 0) {
                                        ship->totalCondition -= projectile->damage;
                                        logEvent("Ship has taken damage.", ship->friendly);
                                    }
                                    
                                    std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                                    std::advance(randomSystem, random0_n(ship->shipSystems.size()));

                                    std::vector<Room>::iterator randomRoom = randomSystem->second->rooms.begin(); 
                                    std::advance(randomRoom, random0_n(randomSystem->second->rooms.size()));

                                    //projectile damage should be modulated by the shields.
                                    std::vector<std::string> damagedPersonnel = randomRoom->dealDamageToRoom(projectile->damage);
                                    for (std::string personnelLogged: damagedPersonnel) {
                                        logEvent(personnelLogged, ship->friendly);
                                    }
                                    ship->totalCondition -= projectile->damage;

                                    for (auto& pair : ship->shipSystems) {
                                        pair.second->setHitbox(ship);
                                        //check for collision and log the string returned by checkCollision

                                        //if a string was returned there was a collision
                                        if (pair.second->checkCollision(disruptor->projectileSprite.getPosition()))
                                            logEvent(pair.second->dealDamageToSystem(projectile->damage), ship->friendly);
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
void Game::fireTorpedo(Ship& firingShip, int hitChance) {
    fireTorpedo(firingShip, (sf::Vector2f)sf::Mouse::getPosition(*window), hitChance);
}

void Game::fireTorpedo(Ship& firingShip, sf::Vector2f targetP, int hitChance) {
    //need to constantly update the sprite object in the Ship object. 
    
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().width / 2});
    sf::Vector2f target = targetP;
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
    if (firingShip.friendly) {
        torpedo->setFriendly();
    }

    for (Ship* ship: allShips) {
        if (!((ship->friendly && firingShip.friendly) || (!ship->friendly && !firingShip.friendly))) {
            sf::FloatRect shipBounds = ship->shipSprite.getGlobalBounds();
            if (shipBounds.contains(target)) {
                for (auto& pair: ship->shipSystems) {
                    std::shared_ptr<System>& system = pair.second; 
                    if (system->checkCollision(target)) {
                        torpedo->targetSystem = system->systemType;
                        torpedo->targetSystemObj = system;
                        torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                        torpedo->targetingSystem = true;
                        break;
                    }

                    
                }
                if (torpedo->targetingSystem == false) {
                    // if the ship is clicked but not a system, just target a random system.
                    std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                    std::advance(randomSystem, random0_n(ship->shipSystems.size()));
                    if (randomSystem != ship->shipSystems.end()) {
                        std::shared_ptr<System>& system = randomSystem->second;
                        torpedo->targetSystem = system->systemType;
                        torpedo->targetSystemObj = system;
                        torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                        torpedo->targetingSystem = true;
                    }
                    
                }
            } else {
                torpedo->hitChance = hitChance * torpedo->hitChanceBase / 100;
                torpedo->targetPos = target;
            }   
        }
    }   

    torpedo->hitChance = hitChance * torpedo->hitChanceBase / 100;
    this->projectilesList.insert(projectilesList.begin(), torpedo);

}

void Game::fireTorpedoSpread(Ship& firingShip, int hitChance) {
    fireTorpedoSpread(firingShip, (sf::Vector2f)sf::Mouse::getPosition(*window), hitChance);
}

void Game::fireTorpedoSpread(Ship& firingShip, sf::Vector2f targetP, int hitChance) {
    
    //need to constantly update the sprite object in the Ship object. 
    
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().width / 2});
    
    float cosValue = cos(firingShip.shipSprite.getRotation() * M_PI / 180);
    float sinValue = sin(firingShip.shipSprite.getRotation() * M_PI / 180);
    
    if (abs(cosValue) < 0.0001) {
        cosValue = 0;
    }
    if (abs(sinValue) < 0.0001) {
        sinValue = 0;
    }

    for (int i = 0; i < 5; i++) {
        sf::Vector2f target = targetP;
        sf::Vector2f directionOfTravel = sf::Vector2f(cosValue, sinValue);
        
        Torpedo* torpedo = new Torpedo("../resource/photontorpedo.png", parentTip.x, parentTip.y,
                                            directionOfTravel, 1000.0, 10);
        if (firingShip.friendly)
            torpedo->setFriendly();
        
        for (Ship* ship: allShips) {
            if (!((ship->friendly && firingShip.friendly) || (!ship->friendly && !firingShip.friendly))) {
                sf::FloatRect shipBounds = ship->shipSprite.getGlobalBounds();
                if (shipBounds.contains(target)) {
                    // if the ship is clicked but not a system, just target a random system.
                    std::map<std::string, std::shared_ptr<System>>::iterator randomSystem = ship->shipSystems.begin();
                    std::advance(randomSystem, random0_n(ship->shipSystems.size()));
                    if (randomSystem != ship->shipSystems.end()) {
                        std::shared_ptr<System>& system = randomSystem->second;
                        torpedo->targetSystem = system->systemType;
                        torpedo->targetSystemObj = system;
                        torpedo->targetPos = torpedo->targetSystemObj->hitbox.getPosition();
                        torpedo->targetingSystem = true;
                    }
                } else {
                    target += sf::Vector2f(randomNegPos() * random0_nInclusive(150), randomNegPos() * random0_nInclusive(150));
                    torpedo->targetPos = target;
                }
            }   
        }

        torpedo->damage = 2;
        torpedo->hitChance = hitChance * torpedo->hitChanceBase / 100;
        if (firingShip.friendly)
            torpedo->setFriendly(); 
        this->projectilesList.insert(projectilesList.begin(), torpedo);
    }
}

void Game::fireDisruptor(Ship& firingShip, int hitChance) {
    fireDisruptor(firingShip, (sf::Vector2f)sf::Mouse::getPosition(*window), hitChance);
}

void Game::fireDisruptor(Ship& firingShip, sf::Vector2f targetP, int hitChance) {
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().width / 2});
    sf::Vector2f target = targetP;
    
    Disruptor* disruptor = new Disruptor("../resource/disruptor.png", parentTip.x, parentTip.y,
                                        target, 1700.0, 6);  

    Disruptor* disruptor2 = new Disruptor("../resource/disruptor.png", parentTip.x, parentTip.y,
                                        target, 1700.0, 6);  
    disruptor2->secondShot = true;
    disruptor2->firingShip = &firingShip;

    if (firingShip.friendly) {
        disruptor->setFriendly();
        disruptor2->setFriendly();
    }

    disruptor->hitChance = hitChance * disruptor->hitChanceBase / 100;
    disruptor2->hitChance = hitChance * disruptor2->hitChanceBase / 100;

    this->projectilesList.insert(projectilesList.begin(), disruptor);
    this->projectilesList.insert(projectilesList.begin(), disruptor2);
}

void Game::moveShip(Ship* ship, sf::Vector2f moveTo) {
    /* Get the position of the player's ship and the position of the mouse as vectors. 
    Find the vector which is the difference between the 2 vectors and normalize it by dividing by length. The vector is normalized so it can be multiplied by a constant speed.
    Move by the difference vector times speed times deltatime. */
    sf::Vector2f movementDistance = (sf::Vector2f) moveTo - ship->shipSprite.getPosition();
    float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
    
    if (length != 0.0f) {
        sf::Vector2f normalizedVector = movementDistance / length;

        float speedTotal = 0;
        int speedCount = 0;
        float operationalCapacity = 0;
        for (auto& pair: ship->shipSystems) {
            std::shared_ptr<System>& system = pair.second;
            Propulsion* prop = dynamic_cast<Propulsion*>(system.get());
            if (prop != nullptr) {
                prop->calculateOperationalCapacity();
                speedTotal += prop->speed;
                operationalCapacity += prop->operationalCapacity;
                speedCount++;
            }
        }
        float speed = 0;
        if (speedCount > 0 && speedTotal > 1) {
            speed = speedTotal / speedCount;
            operationalCapacity /= speedCount;
        } else {
            speed = 0;
            operationalCapacity = 0;
        }

        if (speed < 0 || speedCount <= 0) {
            speed = 0;
        }

        float rotationSpeed = 70 * operationalCapacity / 100;
        //ship's propulsion system speed stat multiplied to give effective speed
        ship->shipSprite.move(normalizedVector * speed * deltaTime);

        /*Find the angle of the distance vector using atan2, and convert to degrees. Then normalize it to be from 0 to 360 degrees. */
        float distanceAngle = (180.0f / M_PI * atan2(normalizedVector.y, normalizedVector.x));
        distanceAngle = std::fmod(distanceAngle + 360.0f, 360.0f);
        //Algorithm for determining if it is closer to rotate clockwise or counterclockwise to the target angle. 
        //Will not trigger if the ship's orientation is within 10 degrees of where the user is currently clicking.
        int cwDistance;
        int ccwDistance;
        if (abs(ship->shipSprite.getRotation() - distanceAngle) > 10) {
            if (distanceAngle >= ship->shipSprite.getRotation()) {
                cwDistance = distanceAngle - ship->shipSprite.getRotation();
                ccwDistance = ship->shipSprite.getRotation() + 360.0f - distanceAngle;
            } else {
                cwDistance = 360.0f - ship->shipSprite.getRotation() + distanceAngle;
                ccwDistance = ship->shipSprite.getRotation() - distanceAngle;
            }

            if (ccwDistance > cwDistance) {
                ship->shipSprite.rotate(rotationSpeed * deltaTime);
            } else if (ccwDistance < cwDistance) {
                ship->shipSprite.rotate(-rotationSpeed * deltaTime);
            }
        }
        
    }
    
}

void Game::movePlayer() {
    if (playerShipObj.totalCondition <= 0) {
        return;
    }
    
    moveShip(playerShipPointer, (sf::Vector2f)sf::Mouse::getPosition(*window));

}

void Game::firePhaser(Ship& firingShip, int hitChance) {
    firePhaser(firingShip, (sf::Vector2f)sf::Mouse::getPosition(*window), hitChance);
}

void Game::firePhaser(Ship& firingShip, sf::Vector2f targetP, int hitChance) {
    sf::Vector2f parentTip = firingShip.shipSprite.getTransform().transformPoint({firingShip.shipSprite.getLocalBounds().height, firingShip.shipSprite.getLocalBounds().width / 2});
    sf::Vector2f target = targetP;
    
    Phaser* phaser = new Phaser("../resource/phaser.png", parentTip.x, parentTip.y,
                                        target, 1700.0, 6);  

    if (firingShip.friendly) {
        phaser->setFriendly();
    }
    phaser->damage = 10;

    phaser->firingShip = &firingShip;

    phaser->hitChance = phaser->hitChanceBase / 100;
    
    this->projectilesList.push_back(phaser);
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
        
        if (playerShipObj.totalCondition > 0) {  
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::Num1)
                {
                    // 1 key was pressed, weapon in slot 1 is now active. 
                    playerShipObj.weaponSelected = 1;
                }

                if (event.key.scancode == sf::Keyboard::Scan::Num2)
                {
                    playerShipObj.weaponSelected = 2;
                }

                if (event.key.scancode == sf::Keyboard::Scan::Num3)
                {
                    playerShipObj.weaponSelected = 3;
                }

                if (event.key.scancode == sf::Keyboard::Scan::Num4)
                {
                    playerShipObj.weaponSelected = 4;   
                }

                pickWeapon(playerShipObj);

                if (event.key.scancode == sf::Keyboard::Scan::Num0)
                {
                    // 1 key was pressed, weapon in slot 1 is now active. For now it will be a photon torpedo.
                    if (debugMode) {
                        debugMode = false; 
                        logEvent("Debug mode off.", true);
                    } else {
                        debugMode = true;
                        logEvent("Debug mode on.", true);
                    }
                        
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
    showRoomDamage();
    renderEnemyHitboxes();

    if (debugMode)
        renderDebugObjects();
    
    sf::View view(sf::FloatRect(0, 0, window->getSize().x + 100, window->getSize().y + 100));
    view.setCenter(playerShipObj.shipSprite.getPosition());
    // Set the view to the window
    this->window->setView(view);
    this->window->display();
    //this->window->setView(view);
    friendlyHitboxes.clear();
    debugHitboxes.clear();
    enemyHitboxes.clear();
}

void Game::logEvent(std::string event, bool friendly) {
    std::tuple<std::string, bool> newEvent = std::tuple<std::string, bool>(event, friendly);
    if (eventLog.size() >= 10) {
        eventLog.pop_back();
    }
    eventLog.insert(eventLog.begin(), newEvent);
}

//create the text at the bottom of the log, and move up by reducing the offset down the screen.
void Game::displayEvents() {
    int positionOffset = 10;
    for (auto& tuple: eventLog) {
        sf::Text text(std::get<0>(tuple), font);
        text.setScale(0.5, 0.5);
        //text will become more transparent as it moves up the log.
        //red or green text based on which faction is hit
        if (std::get<1>(tuple))
            text.setFillColor(sf::Color(255, 100, 100, 25 * positionOffset));
        else {
            text.setFillColor(sf::Color(100, 255, 100, 25 * positionOffset));
        }
        //display relative to view.
        sf::Vector2i viewPosition = sf::Vector2i(0, 12 * positionOffset);
        text.setPosition(window->mapPixelToCoords(viewPosition));
        positionOffset--;
        window->draw(text);
    }
}

void Game::miniTextCreate(std::string text, sf::Vector2f pos) {
    sf::Text miniText(text, font);
    miniText.setScale(0.5, 0.5);
    //text will become more transparent as it moves up the log.
    miniText.setFillColor(sf::Color(255, 255, 255, 255));
    miniText.setPosition(pos);
    miniTextVect.push_back(std::tuple(miniText, 255));
}

void Game::displayMiniText() {
    std::vector<std::tuple<sf::Text, int>>::iterator it;
    for (it = miniTextVect.begin(); it < miniTextVect.end(); it++) {
        auto& tuple = *it;
        if (std::get<1>(tuple) > 0) {
            std::get<0>(tuple).setFillColor(sf::Color(255, 255, 255, std::get<1>(tuple)));
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

bool Game::pickWeapon(Ship& ship) {
    //make sure a weapon is selected and the weapon is actually in the map.
    if (ship.weaponSelected != 0 && ship.weaponSelected <= ship.weaponsComplement.size()) {
        //get the first item of the tuple which contains the weapon type and system
        std::string weaponSelectedString = std::get<0>(ship.weaponsComplement.at(ship.weaponSelected));
        std::string weaponSystem = std::get<1>(ship.weaponsComplement.at(ship.weaponSelected));

        //check if the associated system actually exists.
        if (ship.shipSystems.find(weaponSystem) != ship.shipSystems.end()) {
            std::shared_ptr<System>& system = ship.shipSystems.at(weaponSystem);
            Weapon* wep = dynamic_cast<Weapon*>(system.get());
            if (wep != nullptr) {
                if (wep->ready) {
                    if (ship.friendly)
                        std::cout << weaponSelectedString << " selected" << std::endl;
                    ship.weaponSelectedTuple = std::tuple(weaponSelectedString, weaponSystem);
                    return true;
                } else {
                    if (ship.friendly)
                        if (wep->disabled)
                            std::cout << weaponSelectedString << " is disabled!" << std::endl;
                        else {
                            std::cout << weaponSelectedString << " on cooldown! Time left: " << wep->cooldownThreshold - wep->cooldownTimer << "s" << std::endl;
                        }
                    ship.weaponSelectedTuple = std::tuple("", "");
                    return false;
                }
            }
        }
    }
    ship.weaponSelectedTuple = std::tuple("", "");
    return false;
}

bool Game::pickWeapon(Ship* ship) {
    if (ship != nullptr) {
        return pickWeapon(*ship);
    }
    return false;
}

void Game::useWeapon(Ship& ship) {
    if (std::get<0>(ship.weaponSelectedTuple) != "" && std::get<1>(ship.weaponSelectedTuple) != "") {
        std::string weaponSelectedString = std::get<0>(ship.weaponSelectedTuple);
        std::string weaponSystem = std::get<1>(ship.weaponSelectedTuple);
        bool fired = false;
        int hitChance = ship.shipSystems.at(weaponSystem)->operationalCapacity;
        
        if (weaponSelectedString == "TORPEDO") {
            fireTorpedo(ship, hitChance);
            fired = true;
        }
        if (weaponSelectedString == "DISRUPTOR") {
            fireDisruptor(ship, hitChance); 
            fired = true;
        }
        if (weaponSelectedString == "PHASER") {
            firePhaser(ship, hitChance);
            fired = true;
        }
        if (weaponSelectedString == "TORPEDOSPREAD") {
            fireTorpedoSpread(ship, hitChance); 
            fired = true;
        }

        if (fired) {
            ship.weaponSelected = 0;
            std::shared_ptr<System>& system = ship.shipSystems.at(weaponSystem);
            Weapon* wep = dynamic_cast<Weapon*>(system.get());
            wep->resetTimer();
            ship.weaponSelectedTuple = std::tuple("", "");
        }
    }
}

void Game::useWeapon(Ship* ship, sf::Vector2f enemyPosition) {
    if (std::get<0>(ship->weaponSelectedTuple) != "" && std::get<1>(ship->weaponSelectedTuple) != "") {
        std::string weaponSelectedString = std::get<0>(ship->weaponSelectedTuple);
        std::string weaponSystem = std::get<1>(ship->weaponSelectedTuple);
        int hitChance = ship->shipSystems.at(weaponSystem)->operationalCapacity;

        bool fired = false;
        if (weaponSelectedString == "TORPEDO") {
            fireTorpedo(*ship, enemyPosition, hitChance);
            fired = true;
        }
        if (weaponSelectedString == "DISRUPTOR") {
            fireDisruptor(*ship, enemyPosition, hitChance); 
            fired = true;
        }
        if (weaponSelectedString == "PHASER") {
            firePhaser(*ship, enemyPosition, hitChance);
            fired = true;
        }
        if (weaponSelectedString == "TORPEDOSPREAD") {
            fireTorpedoSpread(*ship, enemyPosition, hitChance); 
            fired = true;
        }

        if (fired) {
            ship->weaponSelected = 0;
            std::shared_ptr<System>& system = ship->shipSystems.at(weaponSystem);
            Weapon* wep = dynamic_cast<Weapon*>(system.get());
            wep->resetTimer();
            ship->weaponSelectedTuple = std::tuple("", "");
        }
    }
}

void Game::makeDecision(Ship* ship) {
    bool noWeaponsReady = true;
    //There should be an evade state, an evasion shooting state, and an aggressive shooting state.
    //Evasion is just about evading enemy fire and perhaps warping away
    //Evasion shooting should have the ship balance between moving and firing at the enemy
    //Aggressive shooting should be the enemy using their weapons as often as possible, pointing towards the enemy.
    float randomX = random0_nInclusive(playerShipPointer->shipSprite.getLocalBounds().width) / 2;
    float randomY = random0_nInclusive(playerShipPointer->shipSprite.getLocalBounds().height) / 2;
    sf::Vector2f randomCoord = sf::Vector2f(playerShipPointer->shipSprite.getPosition().x + randomX, playerShipPointer->shipSprite.getPosition().y + randomY);
    
    for (int wep = 1; wep <= ship->weaponsComplement.size(); wep++) {
        ship->weaponSelected = wep;
        //if we have torpedoes, occasionally pick torpedo spread
        if (pickWeapon(ship)) {
            noWeaponsReady = false;
            break;
        }
    }
    
    if (noWeaponsReady) {
        if (ship->totalCondition <= 25 || ship->shields <= 25) {
            ship->state = "EVAD";
        }
    }
    
    
    if (!noWeaponsReady) {
        if (ship->totalCondition <= 25 || ship->shields <= 25) {
            ship->state = "EVAG";
        } else {
            ship->state = "AGGR";
        }
    }

    sf::Vector2f distToTarget = ship->evadeTargetPosition - ship->shipSprite.getPosition();
    if (std::sqrt(distToTarget.x * distToTarget.x + distToTarget.y + distToTarget.y) < 1) {
        ship->evadeTargetPosition = sf::Vector2f(random0_n(200), random0_n(200));
    }

    ship->state = "EVAG";

    if (ship->state == "AGGR") {
        ship->evadeTargetPosition = sf::Vector2f(-1, -1);
        if (playerShipObj.totalCondition > 0) {
            sf::Vector2f tPos = playerShipPointer->shipSprite.getPosition() - ship->shipSprite.getPosition();
            float rot = atan2(tPos.y, tPos.x) * 180 / M_PI;

            float cwDistance = 0;
            float ccwDistance = 0;
            if (abs(ship->shipSprite.getRotation() - rot) > 10) {
                if (rot >= ship->shipSprite.getRotation()) {
                    cwDistance = rot - ship->shipSprite.getRotation();
                    ccwDistance = ship->shipSprite.getRotation() + 360.0f - rot;
                } else {
                    cwDistance = 360.0f - ship->shipSprite.getRotation() + rot;
                    ccwDistance = ship->shipSprite.getRotation() - rot;
                }

                if (ccwDistance > cwDistance) {
                    ship->shipSprite.rotate(70 * deltaTime);
                } else if (ccwDistance < cwDistance) {
                    ship->shipSprite.rotate(-70 * deltaTime);
                }
            }

            for (int wep = 1; wep <= ship->weaponsComplement.size(); wep++) {
                ship->weaponSelected = wep;
                
                if (pickWeapon(ship)) {
                    if (std::get<0>(ship->weaponSelectedTuple) == "TORPEDO" && random0_nInclusive(1) == 1) {
                        wep++;
                        ship->weaponSelected = wep;
                        pickWeapon(ship);
                    }
                }
                useWeapon(ship, randomCoord);
            }
            
        }
    } else if (ship->state == "EVAG") {
        if (ship->evadeTargetPosition == sf::Vector2f(-1, -1)) {
            ship->evadeTargetPosition = sf::Vector2f(random0_n(1000), random0_n(200));
        }
        for (int wep = 1; wep <= ship->weaponsComplement.size(); wep++) {
            ship->weaponSelected = wep;
            
            if (pickWeapon(ship)) {
                if (std::get<0>(ship->weaponSelectedTuple) == "TORPEDO" && random0_nInclusive(1) == 1) {
                    wep++;
                    ship->weaponSelected = wep;
                    pickWeapon(ship);
                }
            }
            useWeapon(ship, randomCoord);
        }
        moveShip(ship, ship->evadeTargetPosition);
    } else if (ship->state == "EVAD") {
        // just run to a random spot!
        if (ship->evadeTargetPosition == sf::Vector2f(-1, -1)) {
            ship->evadeTargetPosition = sf::Vector2f(random0_n(1000), random0_n(200));
        }
        moveShip(ship, ship->evadeTargetPosition);
    }
}