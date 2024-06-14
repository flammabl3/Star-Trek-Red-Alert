#include "Subsystem.hpp"

Subsystem::Subsystem(std::string name, Personnel* operating)  {
    this->name = name;
    this->operationalCapacity = 100;
    this->operating = operating;
    this->totalCondition = 100;
    this->fire = 0;
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

std::vector<std::string> Subsystem::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;
    // The system's effectiveness should be determined by the capacity and skill of the operator, and the damage it has (or hasn't taken)
    this->operationalCapacity = this->operating->capacity * this->operating->skill * totalCondition;
    return events;
}