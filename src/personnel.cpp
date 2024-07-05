#include <iostream>
#include "personnel.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include "random0_n.hpp"

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
    this->capacity = 1.0;
    this->role = role;
    usingSubsystem = false;
    this->currentState = NORMAL;
    hurtThisFrame = false;
    killedThisFrame = false;
}

Personnel::Personnel(std::string rank, 
                        std::string species, double skill) 
{
    std::string roles[3] = {"Command", "Operations", "Science"};
    this->firstName = "Placeholder";
    this->middleName = "Placeholder";
    this->lastName = "Placeholder";
    this->rank = rank;
    this->species = species;
    this->role = roles[random0_nInclusive(2)];
    this->health = 10.0;
    this->skill = skill;
    this->randomName();
    usingSubsystem = false;
    this->currentState = NORMAL;
    hurtThisFrame = false;
    killedThisFrame = false;
}



Personnel::Personnel(std::string rank, double skill) 
{
    std::string roles[3] = {"Command", "Operations", "Science"};
    this->firstName = "Placeholder";
    this->middleName = "Placeholder";
    this->lastName = "Placeholder";
    this->rank = rank;
    randomSpecies();
    this->role = roles[random0_nInclusive(2)];
    this->health = 10.0;
    this->skill = skill;
    this->randomName();
    usingSubsystem = false;
    this->currentState = NORMAL;
    hurtThisFrame = false;
    killedThisFrame = false;
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

void Personnel::randomName() {
    std::string filename;
    std::vector<std::string> firstNames;
    std::vector<std::string> lastNames;

    filename = "../resource/names/" + this->species + "names.csv";
    
    std::ifstream file(filename);

    if (!file.is_open()) {
        filename = "../resource/names/humanoidnames.csv";
        file.close();  
        file.open(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening " << filename << std::endl;
            return;
        }
    }

    std::string line;
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;

        // Get the first cell from the line (first column)
        std::getline(ss, cell, ',');
        firstNames.push_back(cell);  // Save the cell if you want to use it later
        // Print the first column

        //move over 2 cells to get the last name.
        std::getline(ss, cell, ',');
        std::getline(ss, cell, ',');
        
        lastNames.push_back(cell);
    }

    file.close();  // Close the file

    if (!firstNames.empty() && !lastNames.empty()) {
        firstName = firstNames.at(random0_n(firstNames.size()));
        lastName = lastNames.at(random0_n(lastNames.size()));
        //std::cout << firstName << " " << lastName << std::endl;
    } else {
        std::cerr << "Name vectors are empty." << std::endl;
    }

}

void Personnel::randomSpecies() {
    std::ifstream file("../resource/names/humanoidspecies.csv");
    std::vector<std::string> speciesName;

    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;

        // Get the first cell from the line (first column)
        std::getline(ss, cell, ',');
        speciesName.push_back(cell);  // Save the cell if you want to use it later
    }

    this->species = speciesName.at(random0_n(speciesName.size()));
}

void Personnel::calculateCapacity() {
    this->capacity = mentalStateModifiers[currentState] * health / 10; 
    //mental state should be added.
}