#include "Game.hpp"

#include <iostream>
#include <cmath>
#include <windows.h>
#include <iomanip>
#include <sstream>


#include "Ship.hpp"
#include "InitializeShip.hpp"
#include "Projectile.hpp"
#include "random0_n.hpp"


float rot = 0;

//TODO: 
// Personnel have no mental state system. Not sure how exactly to implement this.
// Improve functions to check for and apply damage, and damage rooms, subsystems, and personnel
// ALL MOVEMENT MUST BE DELTATIMED.

//we never use the operations dept of each crewmember. Additionally, non fed. personnel will not even have this.

/*General path to completion
Systems and damage -> Ship AI -> change views and window sizes (these are partially done)
new ships -> UI -> sounds and animations -> rest of the game (scenarios, menus)

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

    if (!newStarTexture.loadFromFile("../resource/star1.png")) {
        std::cout << "Failed to load star texture." << std::endl;
    }
    if (!newStarTexture2.loadFromFile("../resource/star2.png")) {
        std::cout << "Failed to load star texture." << std::endl;
    }

    debugMode = false;
}

void Game::initWindow() {
    this->videoMode.width = 1366;
    this->videoMode.height = 768;
    this->window = new sf::RenderWindow(sf::VideoMode(1366, 768), "Star Trek: Red Alert");
    this->window->setKeyRepeatEnabled(false);

    sf::Vector2u size = this->window->getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    view = sf::View(sf::FloatRect(0.f, 0.f, 1366.f, 768.f));
    view.zoom(1.0f);

}

void Game::initPlayer() {
    playerShipObj = InitializeShip::makeShip("../resource/ships/FederationEnterpriseD/FederationEnterpriseD.json"); // The Ship object associated with the player's ship will be made the USS enterprise using a function from NCC-1701-D.hpp.
    playerShipObj.shipSprite.setPosition(700, 500);
    playerShipObj.shipSprite.setRotation(270);
    playerShipObj.setFriendly();
    playerShipObj.setSFMLObjects();
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
        if (debugMode)
            rotatePlayerBeforeWarp();
        else
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
    if (playerShipPointer == nullptr)
        return;
    int positionOffset = 5;
    sf::Text text("", font);
    for (auto& pair: playerShipPointer->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        for (Room& room: system->rooms) {
            std::string fireSize;
            if (room.fire == 0)
                fireSize = "no fire";
            else if (0 < room.fire < 25) 
                fireSize = "small fire";
            else if (room.fire < 50) 
                fireSize = "medium fire";
            else if (room.fire < 75)
                fireSize = "large fire";
            else if (room.fire <100)
                fireSize = "huge fire";   
            else
                fireSize = "massive fire";
                
            std::string roomStats = room.roomType + ": " + fireSize + " " + std::to_string((int)room.oxygen) + "% oxygen " + std::to_string((int)system->power) + "% power";
            text.setString(roomStats);
            text.setScale(0.5, 0.5);
            //text will become more transparent as it moves up the log.
            text.setFillColor(sf::Color(255, 255 - room.fire, 255 - room.fire, 255));
            sf::Vector2i viewPosition = sf::Vector2i(0, view.getSize().y - 12 * positionOffset);
            text.setPosition(window->mapPixelToCoords(viewPosition));
            if (positionOffset > 0)
                positionOffset++;
            window->draw(text);
        }
    }
}

void Game::renderDebugObjects() {
    for (sf::RectangleShape& rectangle: debugHitboxes) {
        this->window->draw(rectangle);
    }
}

void Game::renderEnemyHitboxes() {
    for (sf::RectangleShape& rectangle : enemyHitboxes) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldPos = window->mapPixelToCoords(pixelPos, view);
        if (checkCollisionRectangleShape(rectangle, worldPos)) {
            this->window->draw(rectangle);
        }
    }
}


//placeholder code will generate another USS enterprise for shooting at.
void Game::initEnemy() {
    SATHelper sat; //using the same SATHelper multiple times throughout the code causes errors.
    Ship* enemyShipObj = InitializeShip::makeShipPointer("../resource/ships/klingonbirdofprey/klingonbirdofprey.json");
    enemyShipObj->shipSprite.setPosition(300, 100);
    enemyShipObj->shipSprite.setRotation(110);
    enemyShipObj->friendly = false;
    enemyShipObj->setSFMLObjects();
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

        if (!ship->warping && ship->shipSprite.getScale().x > ship->baseScale) {
            ship->shipSprite.scale(0.9, 1);
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
    sf::Vector2f elapsedDistance = projectile->getPosition() - projectile->spawnedAt;
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
        sf::Vector2f elapsedDistance = projectile->getPosition() - projectile->spawnedAt;
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
            sf::Vector2f firingShipPos = projectile->firingShip->getPosition();
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
    projectile->phaserTimer += deltaTime;

    if (projectile->targetShip != nullptr) {
        if (projectile->targetShip->shields > 0) {
            goTo = projectile->targetShip->getPosition() - projectile->targetShip->shieldOffset - projectile->firingShipOffset;;
            projectile->newTarget = goTo;
        } else {
            goTo = projectile->targetShip->getPosition() - projectile->firingShipOffset;;
            projectile->newTarget = goTo;
        }
    } else {
        goTo = projectile->directionOfTravel - projectile->firingShipOffset;;
        projectile->originalTarget = goTo;
    }
    float rotation = (180.0f / M_PI) * atan2(goTo.y, goTo.x);
    projectile->projectileSprite.setRotation(rotation);
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

    if (projectile->phaserScaleX > 300 || projectile->phaserTimer > 5.0) {
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
            if (checkCollisionRectangleShape(ship->shieldRect, projectile->getPosition())) {
                if (!((ship->friendly && projectile->friendly) || (!ship->friendly && !projectile->friendly))) {
                    if (ship->shields > 0) {
                        ship->shields -= projectile->damage;
                        projectileDamage -= ship->shields/8;
                        ship->shieldHit(projectile->getPosition(), true);
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

                        //convert to a decimal of 2 places without c++ 20
                        //cross multiply to get a percent
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(2) << ship->shields * 100 / ship->shieldsBase;
                        logEvent("Shields at " + stream.str() + " percent.", ship->friendly);
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
                            std::stringstream stream;
                            stream << std::fixed << std::setprecision(2) << ship->shields * 100 / ship->shieldsBase;
                            logEvent("Shields at " + stream.str() + " percent.", ship->friendly);
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
                                
                                ship->changeTotalCondition(projectileDamage);
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
                                    if (pair.second->checkCollision(phaser->getPosition()))
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
                                        if (system->checkCollision(projectile->getPosition())) {
                                            if (random0_nInclusive(100) <= torpedo->hitChance) {
                                                std::cout << "SYSTEM HIT" << std::endl;
                                                
                                                
                                                ship->changeTotalCondition(projectileDamage);
                                                logEvent("Ship has taken damage.", ship->friendly);
                                            
                                                logEvent(system->dealDamageToSystem(projectile->damage), ship->friendly);
                                                it = projectilesList.erase(it);
                                                delete projectile;
                                                hit = true;
                                            } else {
                                                std::cout << "MISS" << std::endl;
                                                projectile->missed = true;
                                                miniTextCreate("MISS", projectile->getPosition());
                                            }
                                        }
                                    }
                                }
                            } 
                            
                            if (!torpedo->targetingSystem && !torpedo->missed) {
                                if (random0_nInclusive(100) <= torpedo->hitChance) {
                                    
                                    ship->changeTotalCondition(projectileDamage);
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
                                        if (pair.second->checkCollision(torpedo->getPosition()))
                                            logEvent(pair.second->dealDamageToSystem(projectile->damage), ship->friendly);
                                    }
                                    
                                    // Log before erasing
                                    
                                    
                                    it = projectilesList.erase(it);
                                    delete torpedo;

                                    
                                    continue; // Move to the next iteration
                                } else {
                                    std::cout << "MISS" << std::endl;
                                    torpedo->missed = true;
                                    miniTextCreate("MISS", projectile->getPosition());
                                }
                            }
                        } else if (Disruptor* disruptor = dynamic_cast<Disruptor*>(projectile); disruptor != nullptr) {
                            if (!disruptor->missed) {
                                if (random0_nInclusive(100) <= disruptor->hitChance) {
                                    if (projectile->damage > 0) {
                                        
                                        ship->changeTotalCondition(projectileDamage);
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
                                    ship->changeTotalCondition(projectileDamage);

                                    for (auto& pair : ship->shipSystems) {
                                        pair.second->setHitbox(ship);
                                        //check for collision and log the string returned by checkCollision

                                        //if a string was returned there was a collision
                                        if (pair.second->checkCollision(disruptor->getPosition()))
                                            logEvent(pair.second->dealDamageToSystem(projectile->damage), ship->friendly);
                                    }
                                    
                                    // Log before erasing
                                    
                                    
                                    it = projectilesList.erase(it);
                                    delete disruptor;

                                    continue; // Move to the next iteration
                                } else {
                                    disruptor->missed = true;
                                    std::cout << "MISS" << std::endl;
                                    miniTextCreate("MISS", projectile->getPosition());
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
void Game::fireTorpedo(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip) {
    fireTorpedo(firingShip, window->mapPixelToCoords(sf::Mouse::getPosition(*window)), hitChance, damage, parentTip);
}

void Game::fireTorpedo(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip) {
    //need to constantly update the sprite object in the Ship object. 
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

    torpedo->damage = damage;
    torpedo->hitChance = hitChance * torpedo->hitChanceBase / 100;
    this->projectilesList.insert(projectilesList.begin(), torpedo);

}

void Game::fireTorpedoSpread(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip) {
    fireTorpedoSpread(firingShip, window->mapPixelToCoords(sf::Mouse::getPosition(*window)), hitChance, damage, parentTip);
}

void Game::fireTorpedoSpread(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip) {
    
    //need to constantly update the sprite object in the Ship object. 
    
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

        torpedo->damage = damage / 5;
        torpedo->hitChance = hitChance * torpedo->hitChanceBase / 100;
        if (firingShip.friendly)
            torpedo->setFriendly(); 
        this->projectilesList.insert(projectilesList.begin(), torpedo);
    }
}

void Game::fireDisruptor(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip) {
    fireDisruptor(firingShip, window->mapPixelToCoords(sf::Mouse::getPosition(*window)), hitChance, damage, parentTip);
}

void Game::fireDisruptor(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip) {
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

    disruptor->damage = damage;
    disruptor2->damage = damage;

    disruptor->hitChance = hitChance * disruptor->hitChanceBase / 100;
    disruptor2->hitChance = hitChance * disruptor2->hitChanceBase / 100;

    this->projectilesList.insert(projectilesList.begin(), disruptor);
    this->projectilesList.insert(projectilesList.begin(), disruptor2);
}

void Game::moveShip(Ship* ship, sf::Vector2f moveTo) {
    /* Get the position of the player's ship and the position of the mouse as vectors. 
    Find the vector which is the difference between the 2 vectors and normalize it by dividing by length. The vector is normalized so it can be multiplied by a constant speed.
    Move by the difference vector times speed times deltatime. */
    sf::Vector2f movementDistance = (sf::Vector2f) moveTo - ship->getPosition();
    float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
    
    if (length != 0.0f) {
        sf::Vector2f normalizedVector = movementDistance / length;

        float speedTotal = 0;
        int speedCount = 0;
        float operationalCapacity = 0;

        //the speed we will use is the averaeg of the speed of each nacelle or propulsion system.
        for (auto& pair: ship->shipSystems) {
            std::shared_ptr<System>& system = pair.second;
            Propulsion* prop = dynamic_cast<Propulsion*>(system.get());
            if (prop != nullptr) {
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

void Game::rotateBeforeWarp(Ship* ship, sf::Vector2f moveTo) {
    sf::Vector2f movementDistance = (sf::Vector2f) moveTo - ship->getPosition();
    float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
    sf::Vector2f normalizedVector = movementDistance / length;

    if (length != 0.0f) {
        float operationalCapacity = 0;
        for (auto& pair: ship->shipSystems) {
            std::shared_ptr<System>& system = pair.second;
            Propulsion* prop = dynamic_cast<Propulsion*>(system.get());
            if (prop != nullptr) {
                operationalCapacity += prop->operationalCapacity;
            }
        }
            
        float rotationSpeed = 70 * operationalCapacity / 100;
        //ship's propulsion system speed stat multiplied to give effective speed

        /*Find the angle of the distance vector using atan2, and convert to degrees. Then normalize it to be from 0 to 360 degrees. */
        float distanceAngle = (180.0f / M_PI * atan2(normalizedVector.y, normalizedVector.x));
        distanceAngle = std::fmod(distanceAngle + 360.0f, 360.0f);
        //Algorithm for determining if it is closer to rotate clockwise or counterclockwise to the target angle. 
        //Will not trigger if the ship's orientation is within 10 degrees of where the user is currently clicking.
        int cwDistance;
        int ccwDistance;
        if (abs(ship->shipSprite.getRotation() - distanceAngle) > 10 && !ship->warping) {
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
        } else {
            //extend the ship for the warp effect. 
            moveShipWarp(ship, moveTo);
        }
    }
}

void Game::moveShipWarp(Ship* ship, sf::Vector2f moveTo) {
    ship->warping = true;

    sf::Vector2f movementDistance = (sf::Vector2f) moveTo - ship->getPosition();
    float length = std::sqrt(movementDistance.x * movementDistance.x + movementDistance.y * movementDistance.y);
    sf::Vector2f normalizedVector = movementDistance / length;

    float speedTotal = 0;
    int speedCount = 0;
    float operationalCapacity = 0;
    for (auto& pair: ship->shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        Propulsion* prop = dynamic_cast<Propulsion*>(system.get());
        if (prop != nullptr) {
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

    float rotationSpeed = 10 * operationalCapacity / 100;

    float distanceAngle = (180.0f / M_PI * atan2(normalizedVector.y, normalizedVector.x));
    distanceAngle = std::fmod(distanceAngle + 360.0f, 360.0f);
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

    if ((ship->shipSprite.getScale().x / ship->baseScale) < 4.0f)
        ship->shipSprite.scale(1.01, 1);
    else {
        ship->shipSprite.move(normalizedVector * speed * 100.0f * deltaTime);
    }
}

void Game::movePlayer() {
    if (playerShipPointer == nullptr)
        return;
    if (playerShipObj.totalCondition <= 0) {
        return;
    }
    
    moveShip(playerShipPointer, window->mapPixelToCoords(sf::Mouse::getPosition(*window)));

}

void Game::rotatePlayerBeforeWarp() {
    if (playerShipPointer == nullptr)
        return;
    if (playerShipObj.totalCondition <= 0) {
        return;
    }
    
    rotateBeforeWarp(playerShipPointer, window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
}

void Game::movePlayerWarp() {
    if (playerShipPointer == nullptr)
        return;
    if (playerShipObj.totalCondition <= 0) {
        return;
    }
    
    moveShipWarp(playerShipPointer, window->mapPixelToCoords(sf::Mouse::getPosition(*window)));

}

void Game::firePhaser(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip) {
    firePhaser(firingShip, window->mapPixelToCoords(sf::Mouse::getPosition(*window)), hitChance, damage, parentTip);
}

void Game::firePhaser(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip) {
    sf::Vector2f target = targetP;
    
    Phaser* phaser = new Phaser("../resource/phaser.png", parentTip.x, parentTip.y,
                                        target, 1700.0, 6);  

    if (firingShip.friendly) {
        phaser->setFriendly();
    }
    phaser->damage = damage;

    phaser->firingShip = &firingShip;

    phaser->hitChance = hitChance * phaser->hitChanceBase / 100;
    
    this->projectilesList.push_back(phaser);
}

Game::Game() {
    this->initVariables();
    this->initWindow();
    this->initPlayer();
    this->initEnemy();
    generateStarsStart();
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
        } else if (event.type == sf::Event::Resized) {
            // get the new size of the window
            sf::Vector2u newSize = window->getSize();
            std::cout << "New width: " << newSize.x << " New height: " << newSize.y << std::endl;

            // adjust the viewport
            sf::FloatRect visibleArea(0, 0, newSize.x, newSize.y);
            view = sf::View(visibleArea);
            window->setView(view);
        }
        
        
        if (playerShipObj.totalCondition > 0) {  
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::Num1)
                {
                    // 1 key was pressed, weapon in slot 1 is now active. 
                    playerShipObj.weaponSelected = 1;
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Num2)
                {
                    playerShipObj.weaponSelected = 2;
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Num3)
                {
                    playerShipObj.weaponSelected = 3;
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Num4)
                {
                    playerShipObj.weaponSelected = 4;   
                }

                pickWeapon(playerShipObj);

                if (event.key.scancode == sf::Keyboard::Scan::Num0)
                {
                    if (debugMode) {
                        debugMode = false; 
                        logEvent("Debug mode off.", true);
                    } else {
                        debugMode = true;
                        playerShipObj.shields = 0;
                        logEvent("Debug mode on.", true);
                    }
                        
                }
            } 
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    playerShipObj.warping = false;
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
    this->generateStars();
}

void Game::render() {
    setGameView(playerShipObj.getPosition());
    renderStars();
    renderProjectiles();
    renderPlayer();
    renderEnemy();
    displayEvents();
    displayMiniText();
    showRoomDamage();
    renderEnemyHitboxes();

    if (debugMode) {
        renderCoordinates();
        renderDebugObjects();
    }
        
    
    
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
        int alpha = 25 * positionOffset;
        alpha = (alpha < 0) ? 0 : alpha;
        if (std::get<1>(tuple))
            text.setFillColor(sf::Color(255, 100, 100, alpha));
        else {
            text.setFillColor(sf::Color(100, 255, 100, alpha));
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

        std::shared_ptr<System> wep = ship.shipSystems.at(weaponSystem);
        float damage = std::dynamic_pointer_cast<Weapon>(wep)->damage;

        //projectiles will spawn at the transform of the tip of the system that fired them.
        sf::RectangleShape systemHitbox = ship.shipSystems.at(weaponSystem)->returnHitbox();
        sf::Vector2f parentTip = systemHitbox.getTransform().transformPoint({systemHitbox.getSize().x, systemHitbox.getSize().y / 2});
        
        if (weaponSelectedString == "TORPEDO") {
            fireTorpedo(ship, hitChance, damage, parentTip);
            fired = true;
        }
        if (weaponSelectedString == "DISRUPTOR") {
            fireDisruptor(ship, hitChance, damage, parentTip); 
            fired = true;
        }
        if (weaponSelectedString == "PHASER") {
            firePhaser(ship, hitChance, damage, parentTip);
            fired = true;
        }
        if (weaponSelectedString == "TORPEDOSPREAD") {
            fireTorpedoSpread(ship, hitChance, damage, parentTip); 
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
        std::shared_ptr<System> wep = ship->shipSystems.at(weaponSystem);
        float damage = std::dynamic_pointer_cast<Weapon>(wep)->damage;

        sf::RectangleShape systemHitbox = ship->shipSystems.at(weaponSystem)->returnHitbox();
        sf::Vector2f parentTip = systemHitbox.getTransform().transformPoint({systemHitbox.getSize().x, systemHitbox.getSize().y / 2});
        
        bool fired = false;
        if (weaponSelectedString == "TORPEDO") {
            fireTorpedo(*ship, enemyPosition, hitChance, damage, parentTip);
            fired = true;
        }
        if (weaponSelectedString == "DISRUPTOR") {
            fireDisruptor(*ship, enemyPosition, hitChance, damage, parentTip); 
            fired = true;
        }
        if (weaponSelectedString == "PHASER") {
            firePhaser(*ship, enemyPosition, hitChance, damage, parentTip);
            fired = true;
        }
        if (weaponSelectedString == "TORPEDOSPREAD") {
            fireTorpedoSpread(*ship, enemyPosition, hitChance, damage, parentTip); 
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
    //if the ship's state has changed, then we can set a timer to delay decisions.
    if (ship->decisionTimer > 0) {
        ship->decisionTimer-=deltaTime;
        return;
    }
    if (playerShipPointer == nullptr)
        return;
    bool noWeaponsReady = true;
    bool moveThisFrame = false;
    //There should be an evade state, an evasion shooting state, and an aggressive shooting state.
    //Evasion is just about evading enemy fire and perhaps warping away
    //Evasion shooting should have the ship balance between moving and firing at the enemy
    //Aggressive shooting should be the enemy using their weapons as often as possible, pointing towards the enemy.
    float randomX = random0_nInclusive(playerShipPointer->shipSprite.getLocalBounds().width) / 2;
    float randomY = random0_nInclusive(playerShipPointer->shipSprite.getLocalBounds().height) / 2;
    sf::Vector2f randomCoord = sf::Vector2f(playerShipPointer->getPosition().x + randomX, playerShipPointer->getPosition().y + randomY);

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
            if (ship->state != "EVAD") {
                ship->state = "EVAD";
                ship->decisionTimer = 0.5;
                ship->evadeTargetPosition = sf::Vector2f(ship->getPosition().x - randomNegPos() * (200 + random0_n(400)), ship->getPosition().y - randomNegPos() * (200 + random0_n(400)));
            }
        }
    }
    
    
    if (!noWeaponsReady) {
        
        if (ship->totalCondition <= 25 || ship->shields <= 25) {
            if (ship->state != "EVAG") {
                ship->state = "EVAG";
                ship->decisionTimer = 0.5;
            }
        } else {
            if (ship->state != "AGGR") {
                ship->state = "AGGR";
                ship->decisionTimer = 0.5;
            }
        }
    }


    //only move around if the new generated position is actually farther from the player than the current one. 
    //if the ship is aggressive then try to get close.
    //get within a 25 unit radius.
    if (ship->state == "AGGR") {
        sf::Vector2f toPlayerVect = ship->getPosition() - playerShipPointer->getPosition();
        float toPlayerLength = std::sqrt(toPlayerVect.x * toPlayerVect.x + toPlayerVect.y * toPlayerVect.y);
        toPlayerVect = toPlayerVect / toPlayerLength;
        
        sf::Vector2f closestPoint = sf::Vector2f(toPlayerVect.x * 25.0, toPlayerVect.y * 25.0) + ship->getPosition();
        ship->evadeTargetPosition = window->mapPixelToCoords(sf::Vector2i(closestPoint), view);

        float closestPointLength = std::sqrt(closestPoint.x * closestPoint.x + closestPoint.y * closestPoint.y);

        if (closestPointLength < toPlayerLength) {
            moveThisFrame = true;
        }
    } else {
        //point away and run.
        sf::Vector2f tPos = playerShipPointer->getPosition() - ship->getPosition();
        float rot = atan2(tPos.y, tPos.x) * 180 / M_PI;
        rot += 180;

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

        float radianRot = atan2(tPos.y, tPos.x);
        ship->evadeTargetPosition = ship->getPosition() - sf::Vector2f(cos(radianRot), sin(radianRot));
        moveThisFrame = true;
    }
    


    if (ship->state == "AGGR") {
        if (playerShipObj.totalCondition > 0) {
            sf::Vector2f tPos = playerShipPointer->getPosition() - ship->getPosition();
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
                    //randomly switch between torpedoes and spread. This will need to be changed for weapons without torpedoes, or torpedoes in a different spot.
                    if (std::get<0>(ship->weaponSelectedTuple) == "TORPEDO" && random0_nInclusive(1) == 1) {
                        wep++;
                        ship->weaponSelected = wep;
                        pickWeapon(ship);
                    }
                    useWeapon(ship, randomCoord);
                }
            }

            if (moveThisFrame) {
                moveShip(ship, ship->evadeTargetPosition);
            }
        }
    } else if (ship->state == "EVAG") {
        for (int wep = 1; wep <= ship->weaponsComplement.size(); wep++) {
            ship->weaponSelected = wep;
            
            if (pickWeapon(ship)) {
                if (std::get<0>(ship->weaponSelectedTuple) == "TORPEDO" && random0_nInclusive(1) == 1) {
                    wep++;
                    ship->weaponSelected = wep;
                    pickWeapon(ship);
                    useWeapon(ship, randomCoord);
                }
            }
        }
        if (moveThisFrame)
            moveShip(ship, ship->evadeTargetPosition);
    } else if (ship->state == "EVAD") {
        // just run to a random spot!
        if (moveThisFrame)
            moveShip(ship, ship->evadeTargetPosition);
    }
}

void Game::generateStarsStart() {
    sf::Vector2f size = (view.getSize());
    sf::Vector2f center = view.getCenter();
    generatedChunks.insert(std::pair(std::tuple<int, int>(1, 1), true));

    float left = center.x - size.x / 2;
    float right = center.x + size.x / 2;
    float top = center.y - size.y / 2;
    float bottom = center.y + size.y / 2;

    alreadySeenLeft, alreadySeenTop, alreadySeenRight, alreadySeenBottom = 0;

    for (int i = 20 + random0_nInclusive(20); i > 0; i--) {
        sf::Sprite newStar;
        sf::Texture* textr = (random0_nInclusive(1) == 0) ? &newStarTexture : &newStarTexture2;
        newStar.setTexture(*textr);
        newStar.setColor(sf::Color(random_m_to_n_inclusive(200, 255), 200, random_m_to_n_inclusive(200, 255), random0_nInclusive(255)));
        newStar.setOrigin(newStar.getLocalBounds().width / 2, newStar.getLocalBounds().height / 2);
        newStar.setScale(1 + randomNegPos() * randomfloat0_n(1), 1 + randomNegPos() * randomfloat0_n(1));
        //sf::Vector2f position = window->mapPixelToCoords(sf::Vector2i(random_m_to_n_inclusive(left, right), random_m_to_n_inclusive(bottom, top)));
        newStar.setPosition(sf::Vector2f(random_m_to_n_inclusive(left, right), random_m_to_n_inclusive(top, bottom)));
        starSprites.push_back(newStar);
    }
}

void Game::generateStars() {
    sf::Vector2f size = view.getSize();
    sf::Vector2f center = view.getCenter();

    //divide the world into 1000x1000 chunks. When the player approaches a new chunk they have not seen, generate new stars.   
    //(0 to 1000) is chunk 1. Conversely (0 to -1000) is chunk -1.
    int chunkX = center.x / 1000;
    int chunkY = center.y / 1000;
        
    (chunkX > 0) ? chunkX++ : chunkX--;
    (chunkY > 0) ? chunkY++ : chunkY--;
    //a number between 0 and 1000 divided by 1000 gives us 0, but we want this to be chunk 1.
    if (abs(center.x) < 1000)
        chunkX = (center.x > 0) ? 1 : -1;
    if (abs(center.y) < 1000)
        chunkY = (center.y > 0) ? 1 : -1;
        
    sf::Vector2i chunk = sf::Vector2i(chunkX, chunkY);

    std::vector<sf::Vector2i> chunksToDraw;

    //generate all adjacent chunks in a 3x3, then mark all as generated.
    int adjacentLeft = (chunk.x == 1) ? chunk.x - 2: chunk.x - 1;
    int adjacentRight = (chunk.x == -1) ? chunk.x + 2: chunk.x + 1;
    int adjacentTop = (chunk.y == 1) ? chunk.y - 2: chunk.y - 1;
    int adjacentBottom = (chunk.y == -1) ? chunk.y + 2: chunk.y + 1;

    for (int x = adjacentLeft; x <= adjacentRight; x++) {
        for (int y = adjacentTop; y <= adjacentBottom; y++) {
            if (!(x == 0 && y == 0) && !(x == chunk.x && y == chunk.y)) {
                try {
                    generatedChunks.at(std::tuple<int, int>(x, y));
                } catch (std::exception e) {
                    chunksToDraw.push_back(sf::Vector2i(x, y));
                }
                
            }
        }
    }
    
    for (sf::Vector2i& chunkToDraw: chunksToDraw) {
        generatedChunks.insert_or_assign(std::tuple<int, int>(chunkToDraw.x, chunkToDraw.y), true);
        //the edges of our chunks. If it is negative, the left side will be chunk * 1000 - 1000, and the far will be chunk * 1000.
        //The reverse is true for positive chunks
        int xLeft = (chunkToDraw.x > 0) ? chunkToDraw.x * 1000 - 1000 : chunkToDraw.x * 1000;
        int xRight = (chunkToDraw.x > 0) ? chunkToDraw.x * 1000: chunkToDraw.x * 1000 + 1000;
        int xBottom = (chunkToDraw.y > 0) ? chunkToDraw.y * 1000 - 1000 : chunkToDraw.y * 1000;
        int xTop = (chunkToDraw.y > 0) ? chunkToDraw.y * 1000: chunkToDraw.y * 1000 + 1000;
        for (int i = 20 + random0_nInclusive(20); i > 0; i--) {
            sf::Sprite newStar;
            sf::Texture* textr = (random0_nInclusive(1) == 0) ? &newStarTexture : &newStarTexture2;
            newStar.setTexture(*textr);
            newStar.setColor(sf::Color(random_m_to_n_inclusive(200, 255), 200, random_m_to_n_inclusive(200, 255), random0_nInclusive(255)));
            newStar.setOrigin(newStar.getLocalBounds().width / 2, newStar.getLocalBounds().height / 2);
            newStar.setScale(1 + randomNegPos() * randomfloat0_n(1), 1 + randomNegPos() * randomfloat0_n(1));
            newStar.setPosition(sf::Vector2f(random_m_to_n_inclusive(xLeft, xRight), random_m_to_n_inclusive(xBottom, xTop)));
            starSprites.push_back(newStar);
        }
    }
}

void Game::renderStars() {
    sf::View currentView = window->getView();
    sf::FloatRect viewBounds = sf::FloatRect(currentView.getCenter() - currentView.getSize() / 2.f, currentView.getSize());

    for (sf::Sprite& starSprite: starSprites) {
        if (starSprite.getGlobalBounds().intersects(viewBounds)) {
            window->draw(starSprite);
        }
    }
}


void Game::renderCoordinates() {
    sf::Vector2f mousePosition = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    sf::Text coordinateTextX = sf::Text("X: " + std::to_string(static_cast<int>(mousePosition.x)), font, 15);
    sf::Text coordinateTextY = sf::Text("Y: " + std::to_string(static_cast<int>(mousePosition.y)), font, 15);

    coordinateTextX.setPosition(mousePosition);
    coordinateTextY.setPosition(mousePosition + sf::Vector2f(0, 10));
    
    window->draw(coordinateTextX);
    window->draw(coordinateTextY);
}

void Game::setGameView(sf::Vector2f viewCoordinates) {
    view = sf::View(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
    view.setCenter(viewCoordinates);
    this->window->setView(view);
}