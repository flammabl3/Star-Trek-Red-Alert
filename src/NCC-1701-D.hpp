#ifndef BRIDGE_H
#define BRIDGE_H

#include "ship.hpp"
#include "personnel.hpp"
#include "systems.hpp"
#include <iostream>
#include <string>
#include <map>

class Bridge : public System {
    private:
    

    public:

        Bridge();
        void initBridge();
        
        Personnel personnelList[11];
        Room roomsList[1];
        std::map<std::string, Subsystem> subsystemList;

        
};

Ship getEnterprise();


#endif // BRIDGE_H