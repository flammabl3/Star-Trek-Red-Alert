#include "System.hpp"
#include "Ship.hpp"

System::System(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel){
    this->systemType = systemType;
    this->rooms = rooms;
    this->personnel = personnel;
    this->operationalCapacity = 100;
    this->totalCondition = 100;
    this->power = 100;
    this->disabled = false;
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

void System::setCoordinates(float x, float y, float width, float length) {
    systemX = x;
    systemY = y;
    this->width = width;
    this->length = length;
}

bool System::checkCollision(sf::Vector2f vector) {
    sf::Vector2f translatedVect = vector - hitbox.getPosition();

    float theta = -hitbox.getRotation() * M_PI / 180.0f;
    sf::Vector2f rotatedVect;
    rotatedVect.x = translatedVect.x * cos(theta) - translatedVect.y * sin(theta);
    rotatedVect.y = translatedVect.x * sin(theta) + translatedVect.y * cos(theta);

    float hw = hitbox.getSize().x / 2.0f; 
    float hh = hitbox.getSize().y / 2.0f; 
    if ((-hw <= rotatedVect.x && rotatedVect.x <= hw) && (-hh <= rotatedVect.y && rotatedVect.y <= hh)) {
        return true;
    }
    return false;
}

void Weapon::updateTimer(float deltaTime) {
    // a negative timer means this system does not need to account for time or has no cooldown based mechanics
    if (cooldownThreshold > 0) {
        if (cooldownTimer < cooldownThreshold) {
        cooldownTimer += deltaTime;
        } else if (cooldownTimer > cooldownThreshold) {
            ready = true;
            return;
        }
    }
    //the bool indicates whether the timer has reset.
    ready = false;
}

void Weapon::resetTimer() {
    cooldownTimer = 0;
    ready = false;
}

std::string System::dealDamageToSystem(int damage) {
    std::string events;
    std::vector<Room>::iterator randomRoom = rooms.begin(); 
    std::advance(randomRoom, random0_n(rooms.size()));
    events = this->systemType + " has been hit!";

    randomRoom->dealDamageToRoom(damage);
    return events;
}

std::vector<std::string> System::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;
    if (operationalCapacity > 0) {
        double average = 0;
        for (Room& room: this->rooms) {
            if (power <= 100 && time.asSeconds() > 0.99999 && room.oxygen > power) {
                room.oxygen -= power / 10;
            }
            std::vector<std::string> events2 = room.calculateOperationalCapacity(time);
            events.insert(events.end(), events2.begin(), events2.end());
            average += room.operationalCapacity;
        }
        average /= this->rooms.size();
        this->totalCondition = average;
        //average of totalCondition, average operationalCapacity of all rooms, power of the room.
        //Should not apply to bridge or engineering because they will rely on their own values, causing issues.
        if (systemType == "Bridge") {
            this->power = this->parentShip->shipSystems.at("Engineering")->operationalCapacity;
            this->operationalCapacity = (this->totalCondition + this->power) / 2;
        } else if (systemType == "Engineering") {   
            float bridgeCapacity = this->parentShip->shipSystems.at("Bridge")->operationalCapacity;
            this->operationalCapacity = (this->totalCondition + bridgeCapacity) / 2;
            this->parentShip->warpSpeed = this->parentShip->warpSpeedBase * this->parentShip->selectedWarpLevel * this->operationalCapacity / 100;
        } else {    
            float bridgeCapacity = this->parentShip->shipSystems.at("Bridge")->operationalCapacity;
            this->power = this->parentShip->shipSystems.at("Engineering")->operationalCapacity;
            this->operationalCapacity = (this->totalCondition + this->power + bridgeCapacity) / 3;
        }
    }

    if (operationalCapacity <= 0 && disabled == false) {
        disabled = true;
        operationalCapacity = 0;
        events.push_back(systemType + " is non-functional.");
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

void System::modifyScale(float scale) {
    this->systemX = this->systemX * scale;
    this->systemY = this->systemY * scale;
    this->width = this->width * scale;
    this->length = this->length * scale;
}

Weapon::Weapon(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel) : System(systemType, rooms, personnel) {
    this->cooldownTimer = -1;
    this->ready = false;
}

std::vector<std::string> Weapon::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;
    if (operationalCapacity > 0) {
        double average = 0;
        for (Room& room: this->rooms) {
            if (power <= 100 && time.asSeconds() > 0.99999 && room.oxygen > power) {
                room.oxygen -= power / 10;
            }
            std::vector<std::string> events2 = room.calculateOperationalCapacity(time);
            events.insert(events.end(), events2.begin(), events2.end());
            average += room.operationalCapacity;
        }
        average /= this->rooms.size();
        this->totalCondition = average;
        //average of totalCondition, average operationalCapacity of all rooms, power of the room.
        float bridgeCapacity = this->parentShip->shipSystems.at("Bridge")->operationalCapacity;
        this->power = this->parentShip->shipSystems.at("Engineering")->operationalCapacity;
        this->operationalCapacity = (this->totalCondition + this->power + bridgeCapacity) / 3;
        if (this->operationalCapacity > 0) {
            cooldownThreshold = (cooldownThresholdBase / (this->operationalCapacity / 100));
            this->damage = this->damageBase * this->operationalCapacity / 100;
        } else {
            cooldownThreshold = -1;
        }
    }

    if (operationalCapacity <= 0 && disabled == false) {
        disabled = true;
        operationalCapacity = 0;
        events.push_back(systemType + " is non-functional.");
    }

    return events;
}

System::~System() {};

Propulsion::Propulsion(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel) : System(systemType, rooms, personnel) {
    speed = 0;
};

std::vector<std::string> Propulsion::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;

    double average = 0;
    for (Room& room: this->rooms) {
        if (room.oxygen > 0 && room.oxygen > power && time.asSeconds() > 0.99999) {
            room.oxygen -= power / 10;
        }
        std::vector<std::string> events2 = room.calculateOperationalCapacity(time);
        events.insert(events.end(), events2.begin(), events2.end());
        average += room.operationalCapacity;
    }
    float bridgeCapacity = 0;
    if (operationalCapacity > 0) {
        double average = 0;
        for (Room& room: this->rooms) {
            average += room.operationalCapacity;
        }
        average /= this->rooms.size();
        this->totalCondition = average;
        //average of totalCondition, average operationalCapacity of all rooms, power of the room.
        bridgeCapacity = this->parentShip->shipSystems.at("Bridge")->operationalCapacity;
        this->power = this->parentShip->shipSystems.at("Engineering")->operationalCapacity;
        this->operationalCapacity = (this->totalCondition + this->power + bridgeCapacity) / 3;
        
    }

    if (this->operationalCapacity > 0) {
        speed = baseSpeed * operationalCapacity / 100.0f;
    } else {
        speed = 0;
    }

    if (operationalCapacity <= 0 && disabled == false) {
        disabled = true;
        operationalCapacity = 0;
        events.push_back(systemType + " is non-functional.");
    }
            
    if (operationalCapacity > 200) {
        std::cout << "avg: " << average << std::endl;
        std::cout << "prop cap: " << operationalCapacity << std::endl;
    }

    return events;
}

void Propulsion::calculateOperationalCapacity() {
    if (operationalCapacity > 0) {
        double average = 0;
        for (Room& room: this->rooms) {
            average += room.operationalCapacity;
        }
        average /= this->rooms.size();
        this->totalCondition = average;
        //average of totalCondition, average operationalCapacity of all rooms, power of the room.
        float bridgeCapacity = this->parentShip->shipSystems.at("Bridge")->operationalCapacity;
        this->power = this->parentShip->shipSystems.at("Engineering")->operationalCapacity;
        this->operationalCapacity = (this->totalCondition + this->power + bridgeCapacity) / 3;
        
    }

    if (this->operationalCapacity > 0) {
        speed = baseSpeed * operationalCapacity / 100.0f;
    } else {
        speed = 0;
    }

    if (operationalCapacity <= 0 && disabled == false) {
        disabled = true;
        operationalCapacity = 0;
    }
}