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
        bool usingSubsystem;

        bool hurtThisFrame;
        bool killedThisFrame;

        Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
        std::string species, std::string role, double skill);

        //overload for generic or randomized personnel.
        Personnel(std::string rank, 
        std::string species, double skill);

        Personnel(std::string rank, double skill);

        Personnel() = default;

        std::string getLogName();

        void randomName();

        void calculateCapacity();

        void randomSpecies();
};

