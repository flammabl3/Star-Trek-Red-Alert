#include "ship.hpp"

#include "random0_n.hpp"


Ship::Ship(std::map<std::string, System> shipSystems, int mass, float impulseSpeed, float warpSpeed, std::string name, std::string designation) {
    this->shipSystems = shipSystems;
    this->impulseSpeed = impulseSpeed;
    this->warpSpeed = warpSpeed;
    this->mass = mass;
    this->name = name;
    this->designation = designation;
    //this should vary with different ship strengths actually. the enterprise and a bird of prey should not be equal in combat.
    this->totalCondition = 100;
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
    this->totalCondition = 100;
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


std::string System::checkCollision(Projectile* projectile) {
    std::string outputString;
    sf::Vector2f projectilePos = projectile->projectileSprite.getPosition();

    float angle = hitbox.getRotation() * M_PI / 180;

    float offsetX = systemX * cos(angle) - systemY * sin(angle);
    float offsetY = systemX * sin(angle) + systemY * cos(angle);

    if (this->systemX <= projectilePos.x <= offsetX && this->systemY <= projectilePos.y <= offsetY) {
        if (this->operationalCapacity > 0) {
            this->operationalCapacity -= projectile->damage;
            outputString = this->systemType + " has been hit!";
        //this should eventually be replaced by a function, so that the ship can check for damage every frame, especially from things like fire.
        }
    }
    
    return outputString;
}

void Ship::checkDamage() {
    if (this->totalCondition <= 0) {
        std::cout << "BOOOOOOOOOOOOOOOM" << std::endl;
    } else {
        for (auto& pair: this->shipSystems) {
            System system = pair.second;
            system.calculateOperationalCapacity();
        }
    }
    
    //Use a spritesheet to blow up the ship.
}

Room::Room(std::string roomType, std::vector<Personnel> personnel, std::map<std::string, Subsystem> subsystems)  {
    this-> roomType = roomType;
    this->personnel = personnel;
    this->subsystems = subsystems;
    this->oxygen = 1.0;
    this->operationalCapacity = 1;
    this->totalCondition = 100;
}

Subsystem::Subsystem(std::string name, Personnel operating)  {
    this->name = name;
    this->operationalCapacity = 1;
    this->operating = operating;
    this->totalCondition = 100;
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
    this->operationalCapacity = this->operating.capacity * this->operating.skill * totalCondition / 100;
    // totalCondition is out of 100, and the total should be out of 1, so divide by 100 to get another number scaled around 1.
}

void Room::calculateOperationalCapacity() {
    double average = 0;
    for (auto& pair: this->subsystems) {
        pair.second.calculateOperationalCapacity();
        average += pair.second.operationalCapacity;
    }
    average /= this->subsystems.size();
    this->operationalCapacity = average;
    //capacity will be reduced as the room itself takes damage!
    this->operationalCapacity *= this->totalCondition / 100;
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
        subsystem.operating.health -= finalDamage; //crewmember operating will take damage too. Consider reducing or modifying it by a number from 0 to 1.

        //some characters do not have a last name (klingons, vulcans, data). log their first name. 
        //some HUMANS do not have a last name either. cultures which use patrynomics or otherwise do not have a last name.
        //this should be its own function in personnel later.
        std::string outputString;
        //do not output a string if the crewmember was killed by past damage or if no damage dealt
        if (finalDamage > 0 && subsystem.operating.health + finalDamage > 0) {
            std::cout << subsystem.operating.getLogName() << " has hp: " << subsystem.operating.health << std::endl;
            if (subsystem.operating.health > 0) {
                outputString = subsystem.operating.rank + " " + subsystem.operating.getLogName() + " has become hurt!";
            } else {
                outputString = subsystem.operating.rank + " " + subsystem.operating.getLogName() + " has been killed!";
            }
            outputPersonnel.push_back(outputString);
        }   
        
    }

    this->calculateOperationalCapacity();

    return outputPersonnel;
}

void System::calculateOperationalCapacity() {
    double average = 0;
    for (Room room: this->rooms) {
        room.calculateOperationalCapacity();
        average += room.operationalCapacity;
    }
    average /= this->rooms.size();
    this->operationalCapacity = average;
    this->operationalCapacity *= this->totalCondition / 100;
}