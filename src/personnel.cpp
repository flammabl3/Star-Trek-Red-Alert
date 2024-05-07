#include "personnel.hpp"


Personnel::Personnel(std::string firstName, std::string middleName, std::string lastName, std::string rank, 
                        std::string species, std::string role, double skill) 
{
    this-> firstName = firstName;
    this-> middleName = middleName;
    this-> lastName = lastName;
    this-> rank = rank;
    this->species = species;
    this->health = 10.0;
    this->skill = skill;
}

//capacity will be based on health, skill, mental state. Can exceed 1.

//return first or last name depending on the naming custom of the character.
std::string outputString;
std::string Personnel::getLogName() {
    if (lastName.length() == 0) {
        outputString = firstName;
    } else {
        outputString = lastName;
    }
    
    return(outputString);
}