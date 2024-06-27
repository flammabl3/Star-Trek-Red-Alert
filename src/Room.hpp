#include "Subsystem.hpp"

#ifndef ROOM_HPP
#define ROOM_HPP

class Room {
    private: 

    public:
        std::string roomType;
        std::vector<Personnel*> personnel;
        double oxygen;
        float fire;
        int hullIntegrity;
        double operationalCapacity; 
        double totalCondition;
        std::map<std::string, Subsystem> subsystems;
    
        Room(std::string roomType, std::vector<Personnel*> personnel, std::map<std::string, Subsystem> subsystems);

        Room();

        std::vector<std::string> calculateOperationalCapacity(sf::Time time);

        std::vector<std::string> dealDamageToRoom(int damage);

        std::vector<std::string> fireOxygenPersonnelSwap(sf::Time time);

        std::string messageHurtOrKilled(Personnel* personnel);
};

#endif