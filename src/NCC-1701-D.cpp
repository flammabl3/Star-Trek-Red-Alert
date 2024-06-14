#include "NCC-1701-D.hpp"

//How ships will be stored and selected between, as hpp files.
//This is a terrible way to do it! We should probably have a generic class that takes some kind of data file to initialize ship parts.

void initializeEnterprise::initBridge() {
    rooms.clear();
    personnel.clear();
    std::vector<Personnel*> randomCrew;
    //create lt. commander data
    Personnel* data = new Personnel(std::string("Data"), std::string(""), std::string(""), std::string("Lieutenant Commander"), 
    std::string("Soong-type Android"), std::string("Operations"), 2.0);

    //add him to the personnel list of the bridge.
    personnel.push_back(data);

    Personnel* picard = new Personnel(std::string("Jean-Luc"), std::string(""), std::string("Picard"), std::string("Captain"), 
    std::string("Human"), std::string("Command"), 1.8);

    personnel.push_back(picard);

    Personnel* riker = new Personnel(std::string("William"), std::string("T."), std::string("Riker"), std::string("Commander"), 
    std::string("Human"), std::string("Command"), 1.5);

    personnel.push_back(riker);

    Personnel*worf = new Personnel(std::string("Worf"), std::string(""), std::string(""), std::string("Lieutenant"), 
    std::string("Klingon"), std::string("Operations"), 1.5);

    personnel.push_back(worf);

    Personnel*troi = new Personnel(std::string("Deanna"), std::string(""), std::string("Troi"), std::string("Lt. Commander"), 
    std::string("Human/Betazoid"), std::string("Science"), 1.3);

    personnel.push_back(troi);

    Personnel*ro = new Personnel(std::string("Laren"), std::string(""), std::string("Ro"), std::string("Ensign"), 
    std::string("Bajoran"), std::string("Command"), 1.2);
    
    personnel.push_back(ro);

    
    for (int i = 0; i < 5; i++) {
        Personnel*placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
        randomCrew.push_back(placeholder);
    }
    
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

    Subsystem sciencePanelOne = Subsystem("Science Panel I", randomCrew.at(0));

    subsystemList["Science Panel I"] = sciencePanelOne;

    Subsystem sciencePanelTwo = Subsystem("Science Panel II", randomCrew.at(1));

    subsystemList["Science Panel II"] = sciencePanelTwo;

    Subsystem opsPanel = Subsystem("Operations Panel", randomCrew.at(2));

    subsystemList["Operations Panel"] = opsPanel;

    Subsystem environmentPanel = Subsystem("Environment Panel", randomCrew.at(3));

    subsystemList["Environment Panel"] = environmentPanel;

    Subsystem engineeringPanel = Subsystem("Engineering Panel", randomCrew.at(4));

    subsystemList["Engineering Panel"] = engineeringPanel;
    
    rooms.push_back(Room(std::string("Main Bridge"), personnel, subsystemList));
}

void initializeEnterprise::initForwardPhasers() {
    rooms.clear();
    personnel.clear();
    subsystemList.clear();

    for (int i = 0; i < 5; i++) {
        Personnel*placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
    }

    subsystemList["Forward Phaser Controls"] = Subsystem("Forward Phaser Controls", personnel.at(0));

    subsystemList["Phaser"] = Subsystem("Phaser", personnel.at(1));

    rooms.push_back(Room(std::string("Forward Phasers"), personnel, subsystemList));
}

void initializeEnterprise::initForwardTorpedoes() {
    rooms.clear();
    personnel.clear();
    subsystemList.clear();

    for (int i = 0; i < 3; i++) {
        Personnel*placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
    }

    subsystemList["Forward Torpedo Controls"] = Subsystem("Forward Torpedo Controls", personnel.at(0));

    subsystemList["Torpedo Launcher"] = Subsystem("Torpedo Launcher", personnel.at(1));

    rooms.push_back(Room(std::string("Forward Torpedoes"), personnel, subsystemList));

}
        
void initializeEnterprise::initLeftNacelle() {
    rooms.clear();
    personnel.clear();
    subsystemList.clear();

    for (int i = 0; i < 3; i++) {
        Personnel*placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
    }

    subsystemList["Left Nacelle Controls"] = Subsystem("Left Nacelle Controls", personnel.at(0));

    rooms.push_back(Room(std::string("Nacelle"), personnel, subsystemList));

}

void initializeEnterprise::initRightNacelle() {
    rooms.clear();
    personnel.clear();
    subsystemList.clear();

    for (int i = 0; i < 3; i++) {
        Personnel* placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
    }

    subsystemList["Right Nacelle Controls"] = Subsystem("Right Nacelle Controls", personnel.at(0));

    rooms.push_back(Room(std::string("Nacelle"), personnel, subsystemList));

}

void initializeEnterprise::initEngineering() {
    rooms.clear();
    personnel.clear();
    subsystemList.clear();

    //another set of data structures, since we only have one for the other systems.
    std::vector<Personnel*> personnel2; 
    std::map<std::string, Subsystem> subsystemList2;

    personnel.push_back(new Personnel(std::string("Geordi"), std::string(""), std::string("La Forge"), std::string("Lieutenant Commander"), 
    std::string("Human"), std::string("Operations"), 2.0));

    for (int i = 0; i < 7; i++) {
        Personnel*placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
    }

    subsystemList["Engineering Station I"] = Subsystem("Engineering Station I", personnel.at(0));
    
    subsystemList["Engineering Station II"] = Subsystem("Engineering Station II", personnel.at(1));
    
    subsystemList["Engineering Station III"] = Subsystem("Engineering Station III", personnel.at(2));
    
    subsystemList["Engineering Station IV"] = Subsystem("Engineering Station IV", personnel.at(3));

    rooms.push_back(Room("Engineering Room", personnel, subsystemList));

    for (int i = 0; i < 7; i++) {
        Personnel* placeholder = new Personnel(std::string("Ensign"), 
                                        std::string("Human"), std::string("Command"), 1.0);
        personnel.push_back(placeholder);
        personnel2.push_back(placeholder);
    }
    
    subsystemList2["Warp Core"] = Subsystem("Engineering Station I", personnel.at(0));

    rooms.push_back(Room("Warp Core Room", personnel2, subsystemList2));
}
            

// This is now a helper function that will create all parameters and create a System object with the relevant parameters.
initializeEnterprise::initializeEnterprise() {
    initBridge();
    System* Bridge = new System("Bridge", rooms, personnel);
    // note that the ship coordinates' origin are at the center, not top left
    Bridge->setCoordinates(10, 0, 20, 60);
    systemsList["Bridge"] = std::shared_ptr<System>(Bridge);

    initForwardPhasers();
    Weapon* ForwardPhasers = new Weapon("Forward Phasers", rooms, personnel);
    ForwardPhasers->setCoordinates(25, 0, 15, 50);
    ForwardPhasers->cooldownThresholdBase = 4.0;
    ForwardPhasers->cooldownThreshold = 4.0;
    systemsList["Forward Phasers"] = std::shared_ptr<Weapon>(ForwardPhasers);

    initForwardTorpedoes();
    Weapon* ForwardTorpedoes = new Weapon("Forward Torpedoes", rooms, personnel);
    ForwardTorpedoes->setCoordinates(0, 0, 10, 10);
    ForwardTorpedoes->cooldownThresholdBase = 4.0;
    ForwardTorpedoes->cooldownThreshold = 4.0;
    systemsList["Forward Torpedo Bay"] = std::shared_ptr<Weapon>(ForwardTorpedoes);

    initRightNacelle();
    Propulsion* RightNacelle = new Propulsion("Right Nacelle", rooms, personnel);
    RightNacelle->setCoordinates(-19, -12, 22, 10);
    RightNacelle->speed = 100.0f;
    RightNacelle->baseSpeed = 100.0f;
    systemsList["Right Nacelle"] = std::shared_ptr<Propulsion>(RightNacelle);

    initLeftNacelle();
    Propulsion* LeftNacelle = new Propulsion("Left Nacelle", rooms, personnel);
    RightNacelle->speed = 100.0f;
    RightNacelle->baseSpeed = 100.0f;
    LeftNacelle->setCoordinates(-19, 12, 22, 10);
    systemsList["Left Nacelle"] = std::shared_ptr<Propulsion>(LeftNacelle);

    initEngineering();
    System* Engineering = new System("Engineering", rooms, personnel);
    Engineering->setCoordinates(-20, 0, 20, 20);
    systemsList["Engineering"] = std::shared_ptr<System>(Engineering);
    
}

//our ship should eventually have a size.
Ship getEnterprise() {
    initializeEnterprise enterpriseHelper = initializeEnterprise();
    // all ships will be assumed to have an impulse speed of 0.25c. for now.
    Ship enterprise = Ship(enterpriseHelper.systemsList, 5000000, 0.25, 9.4, 100, std::string("USS Enterprise"), std::string("NCC-1701-D")); 
    enterprise.weaponsComplement[1] = std::tuple("TORPEDO", "Forward Torpedo Bay");
    enterprise.weaponsComplement[2] = std::tuple("TORPEDOSPREAD", "Forward Torpedo Bay");
    enterprise.weaponsComplement[3] = std::tuple("PHASER", "Forward Phasers");

    for (auto& pair: enterprise.shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        for (Room& room: system->rooms) {
            for (auto& pair2: room.subsystems) {
                Subsystem subsystem = pair2.second;
                subsystem.operating->usingSubsystem = true;
            }
        }
    }

    std::cout << "Hello world! ";
    std::cout << enterprise.name << std::endl;

    return enterprise;
}

Ship* getEnterprisePointer() {
    Ship enterprise = getEnterprise();

    for (auto& pair: enterprise.shipSystems) {
        std::shared_ptr<System>& system = pair.second;
        for (Room& room: system->rooms) {
            for (auto& pair2: room.subsystems) {
                Subsystem subsystem = pair2.second;
                subsystem.operating->usingSubsystem = true;
            }
        }
    }

    Ship* enterprisePointer = new Ship(enterprise);


    return enterprisePointer;
}