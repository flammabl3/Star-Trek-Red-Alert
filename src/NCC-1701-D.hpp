#ifndef BRIDGE_H
#define BRIDGE_H

#include "ship.hpp"
#include "personnel.hpp"
#include "systems.hpp"
#include <iostream>
#include <string>
#include <map>

class initializeEnterprise{
    private:
    

    public:
        initializeEnterprise();

        void initBridge();
        void initForwardPhasers();
        void initForwardTorpedoes();
        void initRightNacelle();
        void initLeftNacelle();
        void initEngineering();

        
        std::vector<Room> rooms;
        std::vector<Personnel*> personnel; 
        std::map<std::string, System> systemsList;
        std::map<std::string, Subsystem> subsystemList;

        
};

Ship getEnterprise();

Ship* getEnterprisePointer();

#endif // BRIDGE_H
