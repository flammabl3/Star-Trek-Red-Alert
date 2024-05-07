#include <string>
#pragma once

class Personnel {
    private:
        

    public:
        std::string firstName;
        std::string middleName;
        std::string lastName;
        std::string rank;
        std::string species;
        std::string role;
        double skill;
        double health;
        double capacity; // changes with mental state, health
        
        Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
        std::string species, std::string role, double skill);

        Personnel() = default;

        std::string getLogName();
};