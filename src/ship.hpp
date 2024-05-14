#include <SFML/Graphics.hpp>

#include <memory>
#include <string>
#include "personnel.hpp"
#include "Projectile.hpp"
#include <map>
#include <iostream>
#include <cmath>
#include <random>

#pragma once

class System;
class Room;
class Subsystem;

class Ship
{
    private: // this will change based on the ship
        

    public:
        //SFML objects
        sf::Sprite shipSprite;
        sf::Texture shipTexture;
        sf::FloatRect boundingBox;


        //Internal data
        std::map<std::string, System> shipSystems;
        int mass;
        int shields;
        int power;
        double impulseSpeed;
        double warpSpeed;
        float totalCondition;
        std::string name;
        std::string designation;

        //coordinate of a ship refers to its centre
        // Is this still true?
        int xcoord;
        int ycoord;
        int zcoord;
        float direction;
        
        bool friendly;
        //ships will occupy a rectangular space around their base coordinate.
        int length;
        int width;
        int height;

        Ship(std::map<std::string, System> shipSystems, int mass, float impulseSpeed, float warpSpeed, std::string name, std::string designation);

        Ship();

        void setSFMLObjects(std::string resourcePath);

        sf::FloatRect getBoundingBox();

        void setPos(int x, int y, int z);

        void setSize(int l, int w, int h);

        void setDirection(float direction);

        void render(sf::RenderWindow* window);

        void setFriendly();

        void calculateSystemPositions();

        std::vector<std::string> checkDamage();

        sf::RectangleShape returnHitbox();

        std::vector<std::string> fireOxygenPersonnelSwap();
        //~Ship();
        //define destructor later
        
};

class System {
    private:
    
    public:
        std::string systemType;
        //vector of rooms in the system.
        std::vector<Room> rooms;
        // how well a room is running. Determined by condition of rooms, personnel in rooms.
        double operationalCapacity; 
        // an array of personnel, the current crew of the system. Determined by adding all the personnel lists of each room.
        std::vector<Personnel> personnel; 
        double totalCondition;
        int power;

        sf::Vector2f shipCenter;
        sf::RectangleShape hitbox;
        
        
        //these are the basic coordinates relative to a the ship. i.e., if systemX is 20, then the origin of the
        //System's hitbox will be the ship's origin + 20.
        float systemX;
        float systemY;
        float width;
        float length;

        void setHitbox(Ship* ship);

        sf::RectangleShape returnHitbox();

        void setCoordinates(float x, float y, float width, float length);

        std::string checkCollision(Projectile* projectile);

        std::vector<std::string> calculateOperationalCapacity();

        std::vector<std::string> dealDamageToSystem(int damage);
        
        std::vector<std::string> fireOxygenPersonnelSwap();

        System(std::string systemType, std::vector<Room> rooms, std::vector<Personnel> personnel); 
        // We define a constructor yet never end up using it. Figure this problem out.
        System() = default;
        
};

class Room {
    private: 

    public:
        std::string roomType;
        std::vector<Personnel> personnel;
        double oxygen;
        float fire;
        int hullIntegrity;
        double operationalCapacity; 
        double totalCondition;
        std::map<std::string, Subsystem> subsystems;
    
        Room(std::string roomType, std::vector<Personnel> personnel, std::map<std::string, Subsystem> subsystems);

        Room() {

        }

        std::vector<std::string> calculateOperationalCapacity();

        std::vector<std::string> dealDamageToRoom(int damage);

        std::vector<std::string> fireOxygenPersonnelSwap();
};

class Subsystem { // the individual consoles and parts inside a room.
    private:
        
    public:
        std::string name;
        double operationalCapacity; 
        double totalCondition;
        Personnel operating; // the person at the station
        Subsystem(std::string name, Personnel operating);
        float fire;

        Subsystem() = default;

        std::vector<std::string> calculateOperationalCapacity();

        std::vector<std::string> fireOxygenPersonnelSwap();
};
