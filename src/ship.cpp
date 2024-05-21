#include "ship.hpp"
#include <memory>

#include "random0_n.hpp"


Ship::Ship(std::map<std::string, System> shipSystems, int mass, float impulseSpeed, float warpSpeed, std::string name, std::string designation) {
    this->shipSystems = shipSystems;
    this->impulseSpeed = impulseSpeed;
    this->warpSpeed = warpSpeed;
    this->mass = mass;
    this->name = name;
    this->designation = designation;
    //this should vary with different ship strengths actually. the enterprise and a bird of prey should not be equal in combat.
    this->totalCondition = 200;
    this->shields = 100;
    this->power = 100;
    this->time = clock.getElapsedTime();
}

Ship::Ship() {
    
}
//needs to be here for when a Ship is initialized and then just made to be a certain type of ship through a class definition.

void Ship::setSFMLObjects(std::string resourcePath) {
    if (!this->shipTexture.loadFromFile(resourcePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    shipSprite.setTexture(this->shipTexture);
    shipSprite.setOrigin(shipSprite.getLocalBounds().width / 2, shipSprite.getLocalBounds().height / 2);
}

void Ship::setSize(int l, int w, int h) {
    length = l;
    width = w;
    height = h;
}

void Ship::setDirection(float direction) {
    this->direction = direction;
}

void Ship::setFriendly() {
    friendly = true;
}

//PIP collision for a ship.
bool Ship::checkCollision(sf::Vector2f position) {
    SATHelper sat = SATHelper();
    std::vector<sf::Vector2f> points = sat.getPoints(returnHitbox());
    this->getBoundingBox();
    if (points.at(0).x <= position.x <= points.at(1).x && points.at(0).y <= position.y <= points.at(2).y) {
        return true;
    }
    return false;
}

System::System(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel){
    this->systemType = systemType;
    this->rooms = rooms;
    this->personnel = personnel;
    this->operationalCapacity = 1;
    this->totalCondition = 100;
    this->power = 100;
}


//this does not correctly account for rotated hitboxes.
void System::setHitbox(Ship* ship) {
    sf::Vector2f shipPosition = ship->shipSprite.getPosition();
    float angle = ship->shipSprite.getRotation() * M_PI / 180; // convert to radians

    // Create the hitbox with the appropriate dimensions
    hitbox = sf::RectangleShape(sf::Vector2f(width, length));
    hitbox.setOrigin(width/2, length/2); // set origin to the center of the hitbox

    // Calculate the offset of the hitbox from the ship's position, considering rotation
    float offsetX = systemX * cos(angle) - systemY * sin(angle);
    float offsetY = systemX * sin(angle) + systemY * cos(angle);

    // Set the position and rotation of the hitbox
    hitbox.setPosition(shipPosition.x + offsetX, shipPosition.y + offsetY);
    hitbox.setRotation(ship->shipSprite.getRotation());
}


//DEBUG
sf::RectangleShape System::returnHitbox() {
    hitbox.setFillColor(sf::Color(255,255,255,0));
    hitbox.setOutlineColor(sf::Color(255,255,0,255));
    hitbox.setOutlineThickness(1);
    return(hitbox);
}

sf::RectangleShape Ship::returnHitbox() {
    sf::RectangleShape rectangle(getBoundingBox().getSize());
    rectangle.setPosition(getBoundingBox().left, getBoundingBox().top);
    rectangle.setFillColor(sf::Color(255,255,255,0));
    rectangle.setOutlineColor(sf::Color(255,255,0,255));
    rectangle.setOutlineThickness(1);
    return(rectangle);
}
//DEBUG

void System::setCoordinates(float x, float y, float width, float length) {
    systemX = x;
    systemY = y;
    this->width = width;
    this->length = length;
}


std::string System::checkCollision(Projectile* projectile) {
    std::string outputString;
    sf::Vector2f projectilePos = projectile->projectileSprite.getPosition();
    float x = this->hitbox.getPosition().x;
    float y = this->hitbox.getPosition().y;

    float angle = hitbox.getRotation() * M_PI / 180;

    float offsetX = x * cos(angle) - y * sin(angle);
    float offsetY = x * sin(angle) + y * cos(angle);

    if (x <= projectilePos.x <= offsetX && y <= projectilePos.y <= offsetY) {
        if (this->operationalCapacity > 0) {
            this->operationalCapacity -= projectile->damage;
            outputString = this->systemType + " has been hit!";
        //this should eventually be replaced by a function, so that the ship can check for damage every frame, especially from things like fire.
        }
    }
    
    return outputString;
}

bool System::checkCollision(sf::Vector2f vector) {
    float x = this->hitbox.getPosition().x;
    float y = this->hitbox.getPosition().y;

    float angle = hitbox.getRotation() * M_PI / 180;

    float offsetX = x * cos(angle) - y * sin(angle);
    float offsetY = x * sin(angle) + y * cos(angle);

    if (x <= vector.x <= offsetX && y <= vector.y <= offsetY) {
        return true;
    }
    return false;
}

std::vector<std::string> Ship::checkDamage() {
    if (time.asSeconds() > 0.99999)
        time = clock.restart();
    time = clock.getElapsedTime();

    std::vector<std::string> outputEvents;

    if (this->totalCondition <= 0) {
        //std::cout << "BOOOOOOOOOOOOOOOM" << std::endl;
    } else {
        for (auto& pair: this->shipSystems) {
            System& system = pair.second;
            std::vector<std::string> opEvents = system.calculateOperationalCapacity(time);
            std::vector<std::string> oxyEvents = system.fireOxygenPersonnelSwap(time);

            outputEvents.insert(outputEvents.end(), opEvents.begin(), opEvents.end());
            outputEvents.insert(outputEvents.end(), oxyEvents.begin(), oxyEvents.end());
        }
    }
    
    return outputEvents;
    //Use a spritesheet to blow up the ship.
}

Room::Room(std::string roomType, std::vector<Personnel*> personnel, std::map<std::string, Subsystem> subsystems)  {
    this-> roomType = roomType;
    this->personnel = personnel;
    this->subsystems = subsystems;
    this->oxygen = 100.0;
    this->operationalCapacity = 1;
    this->totalCondition = 100;
    this->fire = 0;
    this->hullIntegrity = 100;
}

Subsystem::Subsystem(std::string name, Personnel* operating)  {
    this->name = name;
    this->operationalCapacity = 1;
    this->operating = operating;
    this->totalCondition = 100;
    this->fire = 0;
} 

void Ship::render(sf::RenderWindow* window) {
    window->draw(this->shipSprite);
}

sf::FloatRect Ship::getBoundingBox() {
    boundingBox = shipSprite.getGlobalBounds();
    return(boundingBox);
}

std::vector<std::string> Subsystem::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;
    // The system's effectiveness should be determined by the capacity and skill of the operator, and the damage it has (or hasn't taken)
    this->operationalCapacity = this->operating->capacity * this->operating->skill * totalCondition / 100;
    // totalCondition is out of 100, and the total should be out of 1, so divide by 100 to get another number scaled around 1.
    return events;
}

std::vector<std::string> Room::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;

    double average = 0;
    for (Personnel* crewmate: personnel) {
        crewmate->calculateCapacity();
    }
    for (auto& pair: this->subsystems) {
        std::vector<std::string> events2 = pair.second.calculateOperationalCapacity(time);
        events.insert(events.end(), events2.begin(), events2.end());
        average += pair.second.operationalCapacity;
    }
    average /= this->subsystems.size();
    this->operationalCapacity = average;
    //capacity will be reduced as the room itself takes damage!
    this->operationalCapacity *= this->totalCondition / 100;

    return events;
}

//damage will be passed to the affected room's subsystems after damage is reduced by the shields.
//damage should also be passed to the room's health itself.
std::vector<std::string> Room::dealDamageToRoom(int damage) {
    std::vector<std::string> outputPersonnel;
    //damage dealt to the room itself.
    //use modifiers to prevent a one hit kill. That being said, an unshielded hit to a room should probably destroy it instantly.
    this->totalCondition -= damage;
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, subsystems.size()); 
    

    //pick a random subsystem to damage. Only one can be damaged in one attack. This may be changed later.
    std::map<std::string, Subsystem>::iterator randomSubsystem = subsystems.begin();
    std::advance(randomSubsystem, random0_n(subsystems.size()));
    
    Subsystem& subsystem = randomSubsystem->second;
    //damage will not be added to a subsystem if it is already destroyed. 
    if (subsystem.totalCondition > 0 && damage > 0) {
        //use random number from 0 to damage for the calculation.
        
        std::uniform_int_distribution<> distr(0, damage); 

        int finalDamage = distr(gen);

        subsystem.totalCondition -= finalDamage;
        subsystem.operating->health -= finalDamage; //crewmember operating will take damage too. Consider reducing or modifying it by a number from 0 to 1.

        //If the damage is significant, a subsystem may be set on fire.
        if (damage > 5) {
            if (fire <= 0) {
                outputPersonnel.push_back(subsystem.name + " has caught fire!");
            }
            subsystem.fire += damage;
            this->fire += damage;
        }

        //some characters do not have a last name (klingons, vulcans, data). log their first name. 
        //some HUMANS do not have a last name either. cultures which use patrynomics or otherwise do not have a last name.
        //this should be its own function in personnel later.
        std::string outputString;
        //do not output a string if the crewmember was killed by past damage or if no damage dealt
        

        if (finalDamage > 0 && subsystem.operating->health + finalDamage > 0) {
            if (subsystem.operating->health > 0) {
                outputString = subsystem.operating->rank + " " + subsystem.operating->getLogName() + " has become hurt!";
            } else {
                outputString = subsystem.operating->rank + " " + subsystem.operating->getLogName() + " has been killed!";
                subsystem.operating->usingSubsystem = false;
            }
            outputPersonnel.push_back(outputString);
        } 
        
    }

    return outputPersonnel;
}

std::vector<std::string> System::dealDamageToSystem(int damage) {
    std::vector<std::string> events;
    std::vector<Room>::iterator randomRoom = rooms.begin(); 
    std::advance(randomRoom, random0_n(rooms.size()));
    
    randomRoom->dealDamageToRoom(damage);
    return events;
}

std::vector<std::string> System::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;

    double average = 0;
    for (Room& room: this->rooms) {
        std::vector<std::string> events2 = room.calculateOperationalCapacity(time);
        events.insert(events.end(), events2.begin(), events2.end());
        average += room.operationalCapacity;
    }
    average /= this->rooms.size();
    this->operationalCapacity = average;
    this->operationalCapacity *= this->totalCondition / 100;

    if (operationalCapacity <= 0) {
        //events.push_back(systemType + " is non-functional.");
    }

    return events;
}

//I don't think this is called. calling fireOxygenPersonnelSwap() is done in checkDamage() for the system objects.
std::vector<std::string> Ship::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;
    for (auto& pair: this->shipSystems) {
        System& system = pair.second;
        std::vector<std::string> events2 = system.fireOxygenPersonnelSwap(time);
        events.insert(events.end(), events2.begin(), events2.end());
    }
    return events;
}

std::vector<std::string> System::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;

    for (auto& room: this->rooms) {
        std::vector<std::string> events2 = room.fireOxygenPersonnelSwap(time);
        events.insert(events.end(), events2.begin(), events2.end());
        //air will drain out of a room with no power.
        if (time.asSeconds() > 0.99999) {
            if (power <= 0) {
                room.oxygen -= 0.25;
            } 
        }
    }
    return events;
}

std::vector<std::string> Room::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;
    if (time.asSeconds() > 0.99999) {
        for (Personnel* crewman: personnel) {
                if (fire > 0 && crewman->health > 0) {
                    if (random0_n(1000-fire*10) == 1) {
                        events.push_back(crewman->rank + " " + crewman->getLogName() + " is being burned!");
                        crewman->health -= 1;
                    }
                }
        }
    }

    
    for (auto& pair: this->subsystems) {
        Subsystem& subsystem = pair.second;

        if (time.asSeconds() > 0.99999) {
            if (hullIntegrity < 80) {
                oxygen -= random0_n((100-hullIntegrity)/ 10);
                // oxygen will drain faster with a more compromised hull. 0 integrity means essentially there's a big hole into space.
            } 
            //If this room is on fire, there is a small chance of the fire spreading to another subsystem.
            if (fire > 0) {
                if (random0_n(1000) == 5) {
                    subsystem.fire += 1;
                    events.push_back("Fire has spread to " + subsystem.name + "!");
                }
                fire += randomfloat0_n(1);
                
                //random chance to be damaged by fire. Higher with more fire.
                totalCondition -= randomfloat0_n(fire/100);
            }
        }
        //replace dead crewmates with living ones.

        //a crewmate will be killed, but their health will reset to 10. What is happening?
        if (subsystem.operating->health <= 0) {
            subsystem.operating->usingSubsystem = false;
            for (Personnel* crewmate: personnel) {
                if (crewmate->health > 0 && crewmate->usingSubsystem == false) {
                    events.push_back(subsystem.operating->getLogName() + " has been replaced by " + crewmate->getLogName());
                    subsystem.operating = crewmate;
                    break;
                }
            }
        }

        std::vector<std::string> events2 = subsystem.fireOxygenPersonnelSwap(time);
        events.insert(events.end(), events2.begin(), events2.end());
    }


    return events;
}

std::vector<std::string> Subsystem::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;

    if (time.asSeconds() > 0.99999) {
        if (fire > 0) {
            fire += randomfloat0_n(1);
            if (randomfloat0_n(1) == 1) {
                int random = random0_n(2);
                if (operating->health > 0) {
                    operating->health -= random;
                    if (random > 0) {
                        events.push_back(operating->rank + " " + operating->getLogName() + " is being burned!");
                    }
                }
                totalCondition -= randomfloat0_n(1);
            }
        }
    }
    //fire will grow randomly each frame, if the subsystem is on fire. 
    //Fire has a chance to harm the operator each frame, and will damage the subsystem.
    return events;
}