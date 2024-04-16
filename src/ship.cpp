#include "ship.hpp"


Ship::Ship(std::map<std::string, System> shipSystems, int mass, int impulseSpeed, int warpSpeed, std::string name, std::string designation) {
    this->shipSystems = shipSystems;
    this->impulseSpeed = impulseSpeed;
    this->warpSpeed = warpSpeed;
    this->mass = mass;
    this->name = name;
    this->designation = designation;
    this->totalCondition = 1;
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

void Ship::setPos(int x, int y, int z) {
    xcoord = x;
    ycoord = y;
    zcoord = z;
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

System::System(std::string systemType, std::vector<Room> rooms, std::vector<Personnel> personnel){
    this->systemType = systemType;
    this->rooms = rooms;
    this->personnel = personnel;
    this->operationalCapacity = 1;
}


//this does not correctly account for rotated hitboxes.
void System::setHitbox(Ship* ship) {
    sf::Vector2f shipHitbox = ship->shipSprite.getPosition();
    hitbox.setRotation(ship->shipSprite.getRotation());
    float angle = ship->shipSprite.getRotation() * M_PI / 180;
    hitbox = sf::RectangleShape(sf::Vector2f(width, length));
    hitbox.setOrigin(width/2, length/2);
    hitbox.setPosition(shipHitbox.x + systemX + width/2, shipHitbox.y - systemY + length/2);
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
    rectangle.setPosition(getBoundingBox().top, getBoundingBox().left);
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


bool System::checkCollision(Projectile* projectile) {
    if (this->operationalCapacity > 0) {
            this->operationalCapacity -= projectile->damage;
        std::cout << this->operationalCapacity << std::endl;
        return true;
        //this should eventually be replaced by a function, so that the ship can check for damage every frame, especially from things like fire.
    }
    return false;
}

void Ship::checkDamage() {
    if (this->totalCondition <= 0) {
        std::cout << "BOOOOOOOOOOOOOOOM" << std::endl;
    }
    //Use a spritesheet to blow up the ship.
}

Room::Room(std::string roomType, std::vector<Personnel> personnel, std::map<std::string, Subsystem> subsystems)  {
    this-> roomType = roomType;
    this->personnel = personnel;
    this->subsystems = subsystems;
    this->oxygen = 1.0;
    this->operationalCapacity = 1;
}

Subsystem::Subsystem(std::string name, Personnel operating)  {
    this->name = name;
    this->operationalCapacity = 1;
    this->operating = operating;
} 

void Ship::render(sf::RenderWindow* window) {
    window->draw(this->shipSprite);
}

sf::FloatRect Ship::getBoundingBox() {
    boundingBox = shipSprite.getGlobalBounds();
    return(boundingBox);
}

void Subsystem::calculateOperationalCapacity() {
    // The system's effectiveness should be determined by the capacity and skill of the operator, and the damage it has (or hasn't taken)
    this->operationalCapacity = this->operating.capacity * this->operating.skill * totalCondition;
}

void Room::calculateOperationalCapacity() {
    double average = 0;
    for (auto& pair: this->subsystems) {
        pair.second.operationalCapacity += average;
    }
    average /= this->subsystems.size();
    this->operationalCapacity = average;
}

void System::calculateOperationalCapacity() {
    double average = 0;
    for (Room room: this->rooms) {
        room.operationalCapacity += average;
    }
    average /= this->rooms.size();
    this->operationalCapacity = average;
}