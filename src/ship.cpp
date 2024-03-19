#include "ship.hpp"

Ship::Ship(std::map<std::string, System> shipSystems, int mass, int impulseSpeed, int warpSpeed, std::string name, std::string designation) {
    this->shipSystems = shipSystems;
    this->impulseSpeed = impulseSpeed;
    this->warpSpeed = warpSpeed;
    this->mass = mass;
    this->name = name;
    this->designation = designation;
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

System::System(std::string systemType, Room rooms[], Personnel personnel[]){
    this->systemType = systemType;
    this->rooms = rooms;
    this->personnel = personnel;
    this->operationalCapacity = 100.0;
}

void System::defineHitbox(int systemX, int systemY) {
    this->systemX = systemX;
    this->systemY = systemY;
}

Room::Room(std::string roomType, Personnel personnel[], std::map<std::string, Subsystem> subsystems)  {
    this-> roomType = roomType;
    this->personnel = personnel;
    this->subsystems = subsystems;
    this->oxygen = 100.0;
    this->operationalCapacity = 100.0;
}

Subsystem::Subsystem(std::string name, Personnel operating)  {
    this->name = name;
    this->operationalCapacity = 100.0;
    this->operating = operating;
} 

void Ship::render(sf::RenderWindow* window) {
    window->draw(this->shipSprite);
}