#include <SFML/Graphics.hpp>

#include <memory>
#include <string>
#include "personnel.hpp"
#include "Projectile.hpp"
#include <map>
#include <iostream>
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
        int impulseSpeed;
        int warpSpeed;
        std::string name;
        std::string designation;

        //coordinate of a ship refers to its centre
        int xcoord;
        int ycoord;
        int zcoord;
        float direction;
        
        bool friendly;
        //ships will occupy a rectangular space around their base coordinate.
        int length;
        int width;
        int height;

        Ship(std::map<std::string, System> shipSystems, int mass, int impulseSpeed, int warpSpeed, std::string name, std::string designation);

        Ship();

        void setSFMLObjects(std::string resourcePath);

        sf::FloatRect getBoundingBox();

        void setPos(int x, int y, int z);

        void setSize(int l, int w, int h);

        void setDirection(float direction);

        void render(sf::RenderWindow* window);

        void setFriendly();

        void calculateSystemPositions();
        //~Ship();
        //define destructor later
        
};

class System {
    private:
    
    public:
        std::string systemType;
        //Array of rooms in the system.
        Room *rooms;
        // how well a room is running. Determined by condition of rooms, personnel in rooms.
        double operationalCapacity; 
        // an array of personnel, the current crew of the system. Determined by adding all the personnel lists of each room.
        Personnel *personnel; 
        
        sf::FloatRect hitbox;
        
        //these are the basic coordinates relative to a the ship. i.e., if systemX is 20, then the origin of the
        //System's hitbox will be the ship's origin + 20.
        float systemX;
        float systemY;
        float width;
        float length;

        void setHitbox(Ship* ship);

        void setCoordinates(float x, float y, float width, float length);

        void checkCollision(Projectile* projectile);

        System(std::string systemType, Room rooms[], Personnel personnel[]); 
        // We define a constructor yet never end up using it. Figure this problem out.
        System() = default;
        
};

class Room {
    private: 

    public:
        std::string roomType;
        Personnel *personnel;
        double oxygen;
        double temperature;
        double operationalCapacity; 
        std::map<std::string, Subsystem> subsystems;
    
        Room(std::string roomType, Personnel personnel[], std::map<std::string, Subsystem> subsystems);

        Room() {

        }

};

class Subsystem { // the individual consoles and parts inside a room.
    private:
        
    public:
        std::string name;
        double operationalCapacity; 
        Personnel operating; // the person at the station
        Subsystem(std::string name, Personnel operating);

        Subsystem() = default;
};
