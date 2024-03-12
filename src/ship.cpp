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
//for some infuriating reason there needs to be a definition for default constructor because it is being called somewhere, and I can't figure out where.

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