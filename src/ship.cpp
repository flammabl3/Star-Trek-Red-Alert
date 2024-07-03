#include "Ship.hpp"
#include <memory>

#include "random0_n.hpp"


Ship::Ship(std::map<std::string, std::shared_ptr<System>> shipSystems, std::string shipTextureName, float impulseSpeed, float warpSpeed, float shields, std::string name, std::string designation) {
    this->shipSystems = shipSystems;
    this->impulseSpeed = impulseSpeed;
    this->warpSpeed = warpSpeed;
    this->shields = shields;
    this->mass = mass;
    this->name = name;
    this->registry = registry;
    this->state = "AGGR";
    this->shipSpritePath = shipTextureName;
    //this should vary with different ship strengths actually. the enterprise and a bird of prey should not be equal in combat.
    this->totalCondition = 50;
    this->shields = 100;
    this->power = 100;
    this->time = clock.getElapsedTime();
    this->shieldOpac = 0;
    this->shieldOffset = sf::Vector2f(0, 0);
    this->evadeTargetPosition = sf::Vector2f(-1, -1);
    this->decisionTimer = 0;
    weaponSelectedTuple = std::tuple("", "");
    weaponSelected = 0;
}

Ship::Ship() {
    
}
//needs to be here for when a Ship is initialized and then just made to be a certain type of ship through a class definition.

void Ship::setSFMLObjects() {
    if (!this->shipTexture.loadFromFile(shipSpritePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    shipSprite.setTexture(this->shipTexture);
    shipSprite.setOrigin(shipSprite.getLocalBounds().width / 2, shipSprite.getLocalBounds().height / 2);

    if (!this->shieldTexture.loadFromFile("../resource/shield.png")) {
        std::cout << "Failed to load." << std::endl;
    }
    shieldSprite.setColor(sf::Color(255, 255, 255, 0));

    shieldSprite.setTexture(this->shieldTexture);
    shieldSprite.setOrigin(50, 50);
    shieldSprite.setScale(0.25, 1);
}

void Ship::setSize(int l, int w, int h) {
    length = l;
    width = w;
    height = h;
}

void Ship::setDirection(float direction) {
    this->direction = direction;
}

void Ship::setFaction(std::string factionName) {
    faction = factionName;
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

void Ship::shieldHit(sf::Vector2f b, bool recalculateOffset) {
    //move the shield sprite to the edge of the shield square radius.
    if (recalculateOffset) 
        shieldOffset = this->shipSprite.getPosition() - b;
    
    sf::Vector2f a = this->shipSprite.getPosition();
    sf::Vector2f c = a - b;

    double angle = atan2(c.y, c.x);
    angle = angle * 180.0 / M_PI;

    //adjust the position to be correct
    shieldSprite.setRotation(angle - 180);
    shieldOpac = 255;
    shieldSprite.setColor(sf::Color(255, 255, 255, shieldOpac));
}

void Ship::shieldOpacMod() {
    shieldSprite.setPosition(this->shipSprite.getPosition() - shieldOffset);
    shieldRect.setPosition(shipSprite.getPosition());
    shieldRect.setRotation(0);
    if (shieldOpac > 0) 
        shieldOpac--;
    shieldSprite.setColor(sf::Color(255, 255, 255, shieldOpac));
}

sf::RectangleShape Ship::setShield(int shieldBubbleRadius) {
    this->shieldBubbleRadius = shieldBubbleRadius;
    sf::Vector2f shipPosition = this->shipSprite.getPosition();
    float angle = this->shipSprite.getRotation() * M_PI / 180; // convert to radians

    // Create the hitbox with the appropriate dimensions
    shieldRect = sf::RectangleShape(sf::Vector2f(shieldBubbleRadius, shieldBubbleRadius));
    shieldRect.setOrigin(shieldBubbleRadius/2, shieldBubbleRadius/2); // set origin to the center of the hitbox

    // Set the position and rotation of the hitbox
    shieldRect.setPosition(shipSprite.getPosition());
    shieldRect.setRotation(shipSprite.getRotation());

    shieldRect.setFillColor(sf::Color(255,255,255,0));
    shieldRect.setOutlineColor(sf::Color(255,255,0,255));
    shieldRect.setOutlineThickness(1);

    return shieldRect;
}

sf::RectangleShape Ship::returnHitbox() {
    sf::RectangleShape rectangle(getBoundingBox().getSize());
    rectangle.setPosition(getBoundingBox().left, getBoundingBox().top);
    rectangle.setFillColor(sf::Color(255,255,255,0));
    rectangle.setOutlineColor(sf::Color(255,255,0,255));
    rectangle.setOutlineThickness(1);
    return(rectangle);
}

std::vector<std::string> Ship::checkDamage() {
    if (time.asSeconds() > 0.99999)
        time = clock.restart();
    time = clock.getElapsedTime();

    std::vector<std::string> outputEvents;
    if (this->totalCondition <= 0) {
        std::cout << "BOOOOOM" << std::endl;
        outputEvents.clear();
        outputEvents.push_back("DESTROYFLAG");
        return outputEvents;
    } else {
        for (auto& pair: this->shipSystems) {
            std::shared_ptr<System> system = pair.second;
            //I wonder if we should have fireOxygenPersonnelSwap() be called as part of each system's calculateOperationalCapacity()...?
            
            std::vector<std::string> opEvents; 
            std::vector<std::string> oxyEvents; 

            Weapon* wep = dynamic_cast<Weapon*>(system.get());
            if (wep != nullptr) {
                opEvents = wep->calculateOperationalCapacity(time);
                oxyEvents = wep->fireOxygenPersonnelSwap(time);
            } else {
                Propulsion* prop = dynamic_cast<Propulsion*>(system.get());
                if (prop != nullptr) {
                    opEvents = prop->calculateOperationalCapacity(time);
                    oxyEvents = prop->fireOxygenPersonnelSwap(time);
                } else {
                    opEvents = system->calculateOperationalCapacity(time);
                    oxyEvents = system->fireOxygenPersonnelSwap(time);
                }
            }

            outputEvents.insert(outputEvents.end(), opEvents.begin(), opEvents.end());
            outputEvents.insert(outputEvents.end(), oxyEvents.begin(), oxyEvents.end());
        }
    }

    return outputEvents;
    //Use a spritesheet to blow up the ship.
}

//I don't think this is called. calling fireOxygenPersonnelSwap() is done in checkDamage() for the system objects.
std::vector<std::string> Ship::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;
    for (auto& pair: this->shipSystems) {
        std::shared_ptr<System> system = pair.second;
        std::vector<std::string> events2 = system->fireOxygenPersonnelSwap(time);
        events.insert(events.end(), events2.begin(), events2.end());
    }
    return events;
}

void Ship::render(sf::RenderWindow* window) {
    window->draw(this->shipSprite);
}

sf::FloatRect Ship::getBoundingBox() {
    boundingBox = shipSprite.getGlobalBounds();
    return(boundingBox);
}

void Ship::changeTotalCondition(int damage) {
    if (totalCondition > 0)
        totalCondition -= damage;
    else {
        totalCondition = 0;
    }

    if (totalCondition < 0) 
        totalCondition = 0;
}

void Ship::changeTotalCondition(float damage) {
    if (totalCondition > 0)
        totalCondition -= damage;
    else {
        totalCondition = 0;
    }

    if (totalCondition < 0) 
        totalCondition = 0;
}

sf::Vector2f Ship::getPosition() {
    return shipSprite.getPosition();
}
