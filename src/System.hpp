#include "Room.hpp"

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#ifndef WEAPON_HPP
#define WEAPON_HPP

#ifndef NACELLE_HPP
#define NACELLE_HPP

class System {
    private:
    
    public:
        std::string systemType;
        //vector of rooms in the system.
        std::vector<Room> rooms;
        // how well a room is running. Determined by condition of rooms, personnel in rooms.
        double operationalCapacity; 
        // an array of personnel, the current crew of the system. Determined by adding all the personnel lists of each room.
        std::vector<Personnel*> personnel; 
        double totalCondition;
        int power;
        bool disabled;

        sf::Vector2f shipCenter;
        sf::RectangleShape hitbox;
        
        Ship* parentShip;
        
        //these are the basic coordinates relative to a the ship. i.e., if systemX is 20, then the origin of the
        //System's hitbox will be the ship's origin + 20.
        float systemX;
        float systemY;
        float width;
        float length;

        void setHitbox(Ship* ship);

        sf::RectangleShape returnHitbox();

        void setCoordinates(float x, float y, float width, float length);

        bool checkCollision(sf::Vector2f vector);

        std::vector<std::string> calculateOperationalCapacity(sf::Time time);

        std::string dealDamageToSystem(int damage);
        
        std::vector<std::string> fireOxygenPersonnelSwap(sf::Time time);

        System(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel); 
        // We define a constructor yet never end up using it. Figure this problem out.
        System() = default;
        
        virtual ~System();

        void modifyScale(float scale);
};

class Weapon : public System {
    public:
        Weapon(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel);

        float cooldownTimer;
        float cooldownThreshold;
        float cooldownThresholdBase;

        void updateTimer(float deltaTime);
        void resetTimer();
        bool ready;

        int damage;
        int damageBase;

        std::vector<std::string> calculateOperationalCapacity(sf::Time time);

};


class Propulsion : public System {
    public:
        Propulsion(std::string systemType, std::vector<Room> rooms, std::vector<Personnel*> personnel);

        float speed;
        float baseSpeed;

        std::vector<std::string> calculateOperationalCapacity(sf::Time time);
        void calculateOperationalCapacity();
};

#endif
#endif
#endif