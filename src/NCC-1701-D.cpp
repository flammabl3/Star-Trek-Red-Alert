#include "ship.h"
#include "personnel.h"
#include "systems.h"
#include <iostream>
#include <string>

class Bridge: public System {
    private:
        

        
    public:
        Personnel personnelList[11];
        Room roomsList[1];
        std::map<std::string, Subsystem> subsystemList;

        void initBridge() {

            //create lt. commander data
            Personnel data = Personnel(std::string("Data"), std::string(""), std::string(""), std::string("Lieutenant Commander"), 
            std::string("Soong-type Android"), std::string("Operations"), 2.0);

            //add him to the personnel list of the bridge.
            personnelList[0] = data;

            Personnel picard = Personnel(std::string("Jean-Luc"), std::string(""), std::string("Picard"), std::string("Captain"), 
            std::string("Human"), std::string("Command"), 1.8);

            personnelList[1] = picard;

            Personnel riker = Personnel(std::string("William"), std::string("T."), std::string("Riker"), std::string("Commander"), 
            std::string("Human"), std::string("Command"), 1.5);

            personnelList[2] = riker;

            Personnel worf = Personnel(std::string("Worf"), std::string(""), std::string(""), std::string("Lieutenant"), 
            std::string("Klingon"), std::string("Operations"), 1.5);

            personnelList[3] = worf;

            Personnel troi = Personnel(std::string("Deanna"), std::string(""), std::string("Troi"), std::string("Lt. Commander"), 
            std::string("Human/Betazoid"), std::string("Science"), 1.3);

            personnelList[4] = troi;

            Personnel ro = Personnel(std::string("Laren"), std::string(""), std::string("Ro"), std::string("Ensign"), 
            std::string("Bajoran"), std::string("Command"), 1.2);
            
            personnelList[5] = ro;

            Personnel placeholder = Personnel(std::string("Place"), std::string(""), std::string("Holder"), std::string("Ensign"), 
            std::string("Human"), std::string("Command"), 1.0);
            
            personnelList[6] = placeholder;
            personnelList[7] = placeholder;
            personnelList[8] = placeholder;
            personnelList[9] = placeholder;
            personnelList[10] = placeholder;
            
            //create the navigator's conn with data on it.
            Subsystem navigatorsConn = Subsystem("Navigator's Conn", data);
            
            subsystemList["Navigator's Conn"] = navigatorsConn;

            Subsystem helmConn = Subsystem("Helmsman's Conn", ro);

            subsystemList["Helmsman's Conn"] = helmConn;

            Subsystem captainsChair = Subsystem("Captain's Chair", picard);

            subsystemList["Captain's Chair"] = captainsChair;

            Subsystem firstOfficerChair = Subsystem("Officer's Chair", riker);

            subsystemList["Officer's Chair"] = firstOfficerChair;

            Subsystem counselorsChair = Subsystem("Counselor's Chair", troi);

            subsystemList["Counselor's Chair"] = counselorsChair;
            
            Subsystem tactical = Subsystem("Tactical", worf);

            subsystemList["Tactical"] = tactical;

            Subsystem sciencePanelOne = Subsystem("Science Panel I", placeholder);

            subsystemList["Science Panel I"] = sciencePanelOne;

            Subsystem sciencePanelTwo = Subsystem("Science Panel II", placeholder);

            subsystemList["Science Panel II"] = sciencePanelTwo;

            Subsystem opsPanel = Subsystem("Operations Panel", placeholder);

            subsystemList["Operations Panel"] = opsPanel;

            Subsystem environmentPanel = Subsystem("Environment Panel", placeholder);

            subsystemList["Environment Panel"] = environmentPanel;

            Subsystem engineeringPanel = Subsystem("Engineering Panel", placeholder);

            subsystemList["Engineering Panel"] = environmentPanel;
            
            roomsList[0] = Room(std::string("Main Bridge"), personnelList, subsystemList);
        }

        //figure out how to make Bridge a System constructor that uses "Bridge" 
        // as systemType and passes rooms from initBridge()
        Bridge() {
            initBridge();
            this->systemType = "Bridge";
            this->personnel = personnelList;
            this->rooms = roomsList;
            this->operationalCapacity = 100.0;
        }

};

Ship getEnterprise() {
    std::map<std::string, System> systemsList;
    Bridge enterpriseBridge = Bridge();
    systemsList["Bridge"] = enterpriseBridge;
    // all ships will be assumed to have an impulse speed of 0.25c. for now.
    Ship enterprise = Ship(systemsList, 5000000, 0.25, 9.4, std::string("USS Enterprise"), std::string("NCC-1701-D")); 

    std::cout << "Hello world! ";
    std::cout << enterprise.name;

    return enterprise;
}