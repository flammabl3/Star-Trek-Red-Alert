#include "InitializeShip.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;

Ship InitializeShip::makeShip(const std::string& jsonFilePath) {
    std::map<std::string, std::shared_ptr<System>> systemsMap;
    std::vector<Room> roomsVect;
    std::vector<Personnel*> personnelVect; 
    std::map<std::string, Subsystem> subsystemMap;
    int generatedRandomPersonnel = 0;

    std::ifstream file(jsonFilePath);

    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        return Ship();
    }

    json shipData;
    file >> shipData;

    for (const auto& system : shipData["systems"]) {
        generatedRandomPersonnel = 0;
        roomsVect.clear();
        personnelVect.clear();
        subsystemMap.clear();
        System* systemPtr;
        if (system["class"] == "System") 
            systemPtr = new System(system["name"], roomsVect, personnelVect);
        if (system["class"] == "Weapon") {
            systemPtr = new Weapon(system["name"], roomsVect, personnelVect);
            dynamic_cast<Weapon*>(systemPtr)->cooldownThreshold = system["cooldownThreshold"];
            dynamic_cast<Weapon*>(systemPtr)->cooldownThresholdBase = system["cooldownThresholdBase"];
            dynamic_cast<Weapon*>(systemPtr)->damage = system["damage"];
            dynamic_cast<Weapon*>(systemPtr)->damageBase = system["damageBase"];
            dynamic_cast<Weapon*>(systemPtr)->hitChanceBase = system["hitChanceBase"];
        }
        if (system["class"] == "Propulsion") {
            systemPtr = new Propulsion(system["name"], roomsVect, personnelVect);
            dynamic_cast<Propulsion*>(systemPtr)->speed = system["speed"];
            dynamic_cast<Propulsion*>(systemPtr)->baseSpeed = system["baseSpeed"];
            //std::cout << dynamic_cast<Propulsion*>(systemPtr)->baseSpeed << std::endl;
        }
        systemPtr->setCoordinates(system["coordinates"]["x"], system["coordinates"]["y"], system["coordinates"]["width"], system["coordinates"]["length"]);
        systemPtr->modifyScale(shipData["scale"]);
        for (const auto& room : system["rooms"]) {
            for (const auto& subsystem : room["subsystems"]) {
                Personnel* randomPersonnel = makeRandomPersonnel(shipData["faction"]);
                randomPersonnel->usingSubsystem = true;
                personnelVect.push_back(randomPersonnel);
                Subsystem subsystemObject = Subsystem(subsystem["name"], randomPersonnel);
                subsystemMap[subsystem["name"]] = subsystemObject;
                //count how many people have been generated so we can meet the target later.
                generatedRandomPersonnel++;
            }
            if (room.contains("desiredRandomPersonnel")) {
                for (; generatedRandomPersonnel < room["desiredRandomPersonnel"]; generatedRandomPersonnel++) {
                    Personnel* randomPersonnel = makeRandomPersonnel(shipData["faction"]);
                    personnelVect.push_back(randomPersonnel);
                }
            }
            if (room.contains("personnel")) {
                for (const auto& personnel : room["personnel"]) {
                    Personnel* personnelPtr = new Personnel(personnel["first name"], 
                                                            personnel["middle name"], 
                                                            personnel["last name"], 
                                                            personnel["rank"], 
                                                            personnel["species"], 
                                                            personnel["department"], 
                                                            personnel["skill"]);
                    personnelVect.push_back(personnelPtr);
                    if (personnel.contains("default subsystem")) {
                        delete subsystemMap[personnel["default subsystem"]].operating;
                        personnelPtr->usingSubsystem = true;
                        subsystemMap[personnel["default subsystem"]].operating = personnelPtr;
                        generatedRandomPersonnel--;
                    }
                }
            }
            Room roomObj = Room(room["name"], personnelVect, subsystemMap);
            systemPtr->rooms.push_back(roomObj);
        }
    
        if (system["class"] == "System") 
            systemsMap[systemPtr->systemType] = std::shared_ptr<System>(systemPtr);
        else if (system["class"] == "Weapon")  
            systemsMap[systemPtr->systemType] = std::shared_ptr<Weapon>(dynamic_cast<Weapon*>(systemPtr));
        else if (system["class"] == "Propulsion") 
            systemsMap[systemPtr->systemType] = std::shared_ptr<Propulsion>(dynamic_cast<Propulsion*>(systemPtr));
    }

    Ship ship = Ship(systemsMap, shipData["shipSpritePath"], ship.totalCondition = shipData["totalCondition"], shipData["warpSpeed"], shipData["shields"], std::string(shipData["name"]), std::string(shipData["registry"]));
    ship.shipSprite.setScale(shipData["scale"], shipData["scale"]);
    ship.shields = shipData["shields"];
    ship.shieldsBase = shipData["shields"];
    ship.baseScale = shipData["scale"];

    std::map<int, std::tuple<std::string, std::string>> weaponsComplementArray;
    for (int i = 0; i < shipData["weaponsComplement"].size(); i++) {
        weaponsComplementArray[i + 1] =  (std::make_tuple(shipData["weaponsComplement"][i]["type"], shipData["weaponsComplement"][i]["system"]));
    }


    ship.weaponsComplement = weaponsComplementArray;
    return ship;
}

Ship* InitializeShip::makeShipPointer(const std::string& jsonFilePath) {
    Ship ship = makeShip(jsonFilePath);
    Ship* shipPointer = new Ship(ship);
    return shipPointer;
}

//make a random ensign and assign them to the newly created subsystem
//1 in 3 chance they are not human.
Personnel* InitializeShip::makeRandomPersonnel(const std::string& faction) {
    Personnel* randomPersonnel;
    if (faction == "Federation") {
        if (random0_n(3) == 0)
            randomPersonnel = new Personnel("Ensign", 1.0);
        else {
            randomPersonnel = new Personnel("Ensign", "Human", 1.0);
        }
    } else {
        randomPersonnel = new Personnel("Ensign", faction, 1.0);
    }
    return randomPersonnel;
}