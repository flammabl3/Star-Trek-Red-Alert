#include <iostream>
#include "personnel.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

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
}

Personnel::Personnel(std::string rank, 
                        std::string species, std::string role, double skill) 
{
    this->firstName = "Placeholder";
    this->middleName = "Placeholder";
    this->lastName = "Placeholder";
    this->rank = rank;
    this->species = species;
    this->health = 10.0;
    this->skill = skill;
    this->randomName();
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

    if (species == "Human") {
        filename = "../resource/humannames.csv";
    }
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
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

    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, firstNames.size() - 1); 
    std::uniform_int_distribution<> distr2(0, lastNames.size() - 1); 

    file.close();  // Close the file

    if (!firstNames.empty() && !lastNames.empty()) {
        firstName = firstNames.at(distr(gen));
        lastName = lastNames.at(distr2(gen));
        //std::cout << firstName << " " << lastName << std::endl;
    } else {
        std::cerr << "Name vectors are empty." << std::endl;
    }

}

void Personnel::calculateCapacity() {
    this->capacity = skill * health / 10; 
    //mental state should be added.
}