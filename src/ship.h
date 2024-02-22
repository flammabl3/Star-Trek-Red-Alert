#include <string>
#include "personnel.h"
#include <map>
#pragma once

class System;
class Room;
class Subsystem;

class Ship
{
    private: // this will change based on the ship
        

    public:

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

        //ships will occupy a rectangular space around their base coordinate.
        int length;
        int width;
        int height;

        Ship(std::map<std::string, System> shipSystems, int mass, int impulseSpeed, int warpSpeed, std::string name, std::string designation) {
            this->shipSystems = shipSystems;
            this->impulseSpeed = impulseSpeed;
            this->warpSpeed = warpSpeed;
            this->mass = mass;
            this->name = name;
            this->designation = designation;
        }

        void setPos(int x, int y, int z) {
            xcoord = x;
            ycoord = y;
            zcoord = z;
        }

        void setSize(int l, int w, int h) {
            length = l;
            width = w;
            height = h;
        }
        //~Ship();

};

class System {
    private:
    public:
        std::string systemType;
        Room *rooms;
        // how well a room is running. Determined by condition of rooms, personnel in rooms.
        double operationalCapacity; 
        // an array of personnel, the current crew of the system. Determined by adding all the personnel lists of each room.
        Personnel *personnel; 
        System(std::string systemType, Room rooms[]) {
            this->systemType = systemType;
            this->rooms = rooms;
            this->operationalCapacity = 100.0;
        }

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
    
        Room(std::string roomType, Personnel personnel[], std::map<std::string, Subsystem> subsystems) {
            this-> roomType = roomType;
            this->personnel = personnel;
            this->subsystems = subsystems;
            this->oxygen = 100.0;
            this->operationalCapacity = 100.0;
        }

};

class Subsystem { // the individual consoles and parts inside a room.
    private:
        
    public:
        std::string name;
        double operationalCapacity; 
        Personnel operating; // the person at the station
        Subsystem(std::string name, Personnel operating) {
            this->name = name;
            this->operationalCapacity = 100.0;
            this->operating = operating;
        }

        Subsystem() = default;
};
