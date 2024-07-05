#include "Ship.hpp"
#include "Personnel.hpp"
#include <iostream>
#include <string>
#include <map>
#include <memory>

class InitializeShip {
    public:
        static Ship makeShip(const std::string& jsonFilePath);
        static Ship* makeShipPointer(const std::string& jsonFilePath);
        static Personnel* makeRandomPersonnel(const std::string& jsonFilePath);
};

