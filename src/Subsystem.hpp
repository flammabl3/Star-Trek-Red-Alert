#include <SFML/Graphics.hpp>

#include <memory>
#include <string>
#include "personnel.hpp"
#include "Projectile.hpp"
#include <map>
#include <iostream>
#include <cmath>
#include <random>
#include "SeparateAxisTheorem.hpp"
#include "random0_n.hpp"

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

class Subsystem { // the individual consoles and parts inside a room.
    private:
        
    public:
        std::string name;
        double operationalCapacity; 
        double totalCondition;
        Personnel* operating; // the person at the station
        Subsystem(std::string name, Personnel* operating);
        float fire;

        Subsystem() = default;

        std::vector<std::string> calculateOperationalCapacity(sf::Time time);

        std::vector<std::string> fireOxygenPersonnelSwap(sf::Time time);
};

#endif