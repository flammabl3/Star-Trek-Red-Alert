#include <string>
#pragma once

class Personnel {
    private:
        std::string firstName;
        std::string middleName;
        std::string lastName;
        std::string rank;
        std::string species;
        std::string role;
        double skill;
        double health;
        double capacity; // changes with mental state, health

    public:
        Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
        std::string species, std::string role, double skill) {
            this-> firstName = firstName;
            this-> middleName = middleName;
            this-> lastName = lastName;
            this-> rank = rank;
            this->species = species;
            this->health = 100.0;
        }

        Personnel() = default;
};