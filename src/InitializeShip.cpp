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
        }
        if (system["class"] == "Propulsion") {
            systemPtr = new Propulsion(system["name"], roomsVect, personnelVect);
            dynamic_cast<Propulsion*>(systemPtr)->speed = system["speed"];
            dynamic_cast<Propulsion*>(systemPtr)->baseSpeed = system["baseSpeed"];
            //std::cout << dynamic_cast<Propulsion*>(systemPtr)->baseSpeed << std::endl;
        }
        systemPtr->setCoordinates(system["coordinates"]["x"], system["coordinates"]["y"], system["coordinates"]["width"], system["coordinates"]["length"]);
        for (const auto& room : system["rooms"]) {
            for (const auto& subsystem : room["subsystems"]) {
                //make a random ensign and assign them to the newly created subsystem
                Personnel* randomPersonnel = new Personnel("Ensign", "Human", 1.0);
                randomPersonnel->usingSubsystem = true;
                personnelVect.push_back(randomPersonnel);
                Subsystem subsystemObject = Subsystem(subsystem["name"], randomPersonnel);
                subsystemMap[subsystem["name"]] = subsystemObject;
                //count how many people have been generated so we can meet the target later.
                generatedRandomPersonnel++;
            }
            if (room.contains("desiredRandomPersonnel")) {
                for (; generatedRandomPersonnel < room["desiredRandomPersonnel"]; generatedRandomPersonnel++) {
                    Personnel* randomPersonnel = new Personnel("Ensign", "Human", 1.0);
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

    Ship ship = Ship(systemsMap, shipData["shipSpritePath"], shipData["impulseSpeed"], shipData["warpSpeed"], shipData["shields"], std::string(shipData["name"]), std::string(shipData["registry"]));
    
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