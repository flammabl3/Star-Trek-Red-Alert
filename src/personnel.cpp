#include "personnel.hpp"


Personnel::Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
                        std::string species, std::string role, double skill) 
{
    this-> firstName = firstName;
    this-> middleName = middleName;
    this-> lastName = lastName;
    this-> rank = rank;
    this->species = species;
    this->health = 100.0;
    this->skill = skill;
}

//capacity will be based on health, skill, mental state. Can exceed 1.