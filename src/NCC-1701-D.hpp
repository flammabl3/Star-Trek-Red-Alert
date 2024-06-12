#ifndef BRIDGE_H
#define BRIDGE_H

#include "Ship.hpp"
#include "personnel.hpp"
#include <iostream>
#include <string>
#include <map>
#include <memory>

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
        std::map<std::string, std::shared_ptr<System>> systemsList;
        std::map<std::string, Subsystem> subsystemList;

        std::map<int, Projectile> weaponsComplement;
        
};

Ship getEnterprise();

Ship* getEnterprisePointer();

#endif // BRIDGE_H
