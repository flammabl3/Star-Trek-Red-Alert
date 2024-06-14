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
        enum mentalState {
            NORMAL = 0,
            STRESSED = 1,
            INSANE = 2,
            INSPIRED = 3,
            DESPAIR = 4
        };
        
        //the multipliers our mental states will confer
        float mentalStateModifiers[5] = {1.0, 0.8, 0.1, 1.2, 0.5};

        mentalState currentState;

        Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
        std::string species, std::string role, double skill);

        //overload for generic or randomized personnel.
        Personnel(std::string rank, 
        std::string species, std::string role, double skill);

        Personnel() = default;

        std::string getLogName();

        void randomName();

        void calculateCapacity();

};

