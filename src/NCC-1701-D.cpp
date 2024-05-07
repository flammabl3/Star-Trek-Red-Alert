    #include "NCC-1701-D.hpp"

    //How ships will be stored and selected between, as hpp files.
    //This is a terrible way to do it! We should probably have a generic class that takes some kind of data file to initialize ship parts.

    void initializeEnterprise::initBridge() {
        rooms.clear();
        personnel.clear();
        std::vector<Personnel> randomCrew;
        //create lt. commander data
        Personnel data = Personnel(std::string("Data"), std::string(""), std::string(""), std::string("Lieutenant Commander"), 
        std::string("Soong-type Android"), std::string("Operations"), 2.0);

        //add him to the personnel list of the bridge.
        personnel.push_back(data);

        Personnel picard = Personnel(std::string("Jean-Luc"), std::string(""), std::string("Picard"), std::string("Captain"), 
        std::string("Human"), std::string("Command"), 1.8);

        personnel.push_back(picard);

        Personnel riker = Personnel(std::string("William"), std::string("T."), std::string("Riker"), std::string("Commander"), 
        std::string("Human"), std::string("Command"), 1.5);

        personnel.push_back(riker);

        Personnel worf = Personnel(std::string("Worf"), std::string(""), std::string(""), std::string("Lieutenant"), 
        std::string("Klingon"), std::string("Operations"), 1.5);

        personnel.push_back(worf);

        Personnel troi = Personnel(std::string("Deanna"), std::string(""), std::string("Troi"), std::string("Lt. Commander"), 
        std::string("Human/Betazoid"), std::string("Science"), 1.3);

        personnel.push_back(troi);

        Personnel ro = Personnel(std::string("Laren"), std::string(""), std::string("Ro"), std::string("Ensign"), 
        std::string("Bajoran"), std::string("Command"), 1.2);
        
        personnel.push_back(ro);

        
        for (int i = 0; i < 5; i++) {
            Personnel placeholder = Personnel(std::string("Ensign"), 
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
    }
            

                

    // This is now a helper function that will create all parameters and create a System object with the relevant parameters.
    initializeEnterprise::initializeEnterprise() {
        initBridge();
        System Bridge = System("Bridge", rooms, personnel);
        // note that the ship coordinates' origin may be at the center, not top left
        Bridge.setCoordinates(10, 0, 20, 40);
        systemsList["Bridge"] = Bridge;

        //to be implemented later.
        /*
        systemsList["Forward Phasers"];
        systemsList["Forward Torpedo Bay"];
        systemsList["Right Nacelle"];
        systemsList["Left Nacelle"];
        systemsList["Engineering"];
        */
    }

    //our ship should eventually have a size.
    Ship getEnterprise() {
        initializeEnterprise enterpriseHelper = initializeEnterprise();
        // all ships will be assumed to have an impulse speed of 0.25c. for now.
        Ship enterprise = Ship(enterpriseHelper.systemsList, 5000000, 0.25, 9.4, std::string("USS Enterprise"), std::string("NCC-1701-D")); 

        std::cout << "Hello world! ";
        std::cout << enterprise.name << std::endl;

        return enterprise;
    }

    Ship* getEnterprisePointer() {
        Ship enterprise = getEnterprise();
        Ship* enterprisePointer = new Ship(enterprise);

        return enterprisePointer;
    }