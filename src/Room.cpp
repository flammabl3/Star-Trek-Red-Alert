#include "Room.hpp"

Room::Room(std::string roomType, std::vector<Personnel*> personnel, std::map<std::string, Subsystem> subsystems)  {
    this-> roomType = roomType;
    this->personnel = personnel;
    this->subsystems = subsystems;
    this->oxygen = 100.0;
    this->operationalCapacity = 1.0;
    this->totalCondition = 100.0;
    this->fire = 0;
    this->hullIntegrity = 100;
}

Room::Room() {
    roomType = "Unknown";
    oxygen = 0.0;
    fire = 0.0;
    hullIntegrity = 0;
    operationalCapacity = 0.0;
    totalCondition = 0.0;
}

//damage will be passed to the affected room's subsystems after damage is reduced by the shields.
//damage should also be passed to the room's health itself.
std::vector<std::string> Room::dealDamageToRoom(int damage) {
    std::vector<std::string> outputPersonnel;
    //damage dealt to the room itself.
    //use modifiers to prevent a one hit kill. That being said, an unshielded hit to a room should probably destroy it instantly.
    if (totalCondition > 0)
        this->totalCondition -= damage;
    else {
        this->totalCondition = 0;
    }
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, subsystems.size()); 
    

    //pick a random subsystem to damage. Only one can be damaged in one attack. This may be changed later.
    std::map<std::string, Subsystem>::iterator randomSubsystem = subsystems.begin();
    std::advance(randomSubsystem, random0_n(subsystems.size()));
    
    Subsystem& subsystem = randomSubsystem->second;
    //damage will not be added to a subsystem if it is already destroyed. 
    if (subsystem.totalCondition > 0 && damage > 0) {
        //use random number from 0 to damage for the calculation.
        
        std::uniform_int_distribution<> distr(0, damage); 

        int finalDamage = distr(gen);

        if (subsystem.totalCondition > 0) {
            subsystem.totalCondition -= finalDamage;
        } else {
            subsystem.totalCondition = 0;
        }

        if (subsystem.operating->health > 0) {
            subsystem.operating->health -= finalDamage; //crewmember operating will take damage too. Consider reducing or modifying it by a number from 0 to 1.
        } else {
            subsystem.operating->health = 0;
        }
            

        //If the damage is significant, a subsystem may be set on fire.
        if (damage > 5) {
            if (fire <= 0) {
                outputPersonnel.push_back(subsystem.name + " has caught fire!");
            }
            subsystem.fire += damage;
            this->fire += damage;
        }

        //some characters do not have a last name (klingons, vulcans, data). log their first name. 
        //some HUMANS do not have a last name either. cultures which use patrynomics or otherwise do not have a last name.
        //this should be its own function in personnel later.
        std::string outputString;
        //do not output a string if the crewmember was killed by past damage or if no damage dealt
        

        if (finalDamage > 0 && subsystem.operating->health + finalDamage > 0) {
            if (subsystem.operating->health > 0) {
                outputString = subsystem.operating->rank + " " + subsystem.operating->getLogName() + " has become hurt!";
            } else {
                outputString = subsystem.operating->rank + " " + subsystem.operating->getLogName() + " has been killed!";
                subsystem.operating->usingSubsystem = false;
            }
            outputPersonnel.push_back(outputString);
        } 
        
    }

    return outputPersonnel;
}

std::vector<std::string> Room::calculateOperationalCapacity(sf::Time time) {
    std::vector<std::string> events;

    double average = 0;
    for (Personnel* crewmate: personnel) {
        crewmate->calculateCapacity();
    }
    for (auto& pair: this->subsystems) {
        std::vector<std::string> events2 = pair.second.calculateOperationalCapacity(time);
        events.insert(events.end(), events2.begin(), events2.end());
        average += pair.second.operationalCapacity;
    }
    if (this->subsystems.size() > 0 && average > 0)
        average /= this->subsystems.size();
    //capacity will be reduced as the room itself takes damage!
    this->operationalCapacity = average * this->totalCondition / 100;
    return events;
}

std::vector<std::string> Room::fireOxygenPersonnelSwap(sf::Time time) {
    std::vector<std::string> events;
    if (time.asSeconds() > 0.99999) {
        for (Personnel* crewman: personnel) {
            if (fire > 0 && crewman->health > 0) {
                if (random0_n(1000-fire*10) == 1) {
                    events.push_back(crewman->rank + " " + crewman->getLogName() + " is being burned!");
                    crewman->health -= 1;
                }
            }
            
            if (oxygen <= 0) {
                for (Personnel* crewmate: personnel) {
                    if (crewmate->health > 0)
                        crewmate->health -= 10;
                }
            }
        }
    } 
    
    for (auto& pair: this->subsystems) {
        Subsystem& subsystem = pair.second;

        if (time.asSeconds() > 0.99999) {
            if (hullIntegrity < 80) {
                oxygen -= random0_n((100-hullIntegrity)/ 10);
                // oxygen will drain faster with a more compromised hull. 0 integrity means essentially there's a big hole into space.
            } 
            //If this room is on fire, there is a small chance of the fire spreading to another subsystem.
            if (fire > 0 && totalCondition > 0) {
                if (random0_n(1000) == 5) {
                    subsystem.fire += 1;
                    events.push_back("Fire has spread to " + subsystem.name + "!");
                }
                fire += randomfloat0_n(1);
                
                //random chance to be damaged by fire. Higher with more fire.
                double fireDmg = (fire/100 > 1) ? randomfloat0_n(fire/100) : 0;
                totalCondition -= fireDmg;
            }
        }
        //replace dead crewmates with living ones.

        if (subsystem.operating->health <= 0) {
            subsystem.operating->usingSubsystem = false;
            for (Personnel* crewmate: personnel) {
                if (crewmate->health > 0 && crewmate->usingSubsystem == false) {
                    events.push_back(subsystem.operating->rank + " " + subsystem.operating->getLogName() + " has been replaced by " + subsystem.operating->rank + " " + crewmate->getLogName() + ".");
                    subsystem.operating = crewmate;
                    break;
                } 
            }
        }

        std::vector<std::string> events2 = subsystem.fireOxygenPersonnelSwap(time);
        events.insert(events.end(), events2.begin(), events2.end());
    }


    return events;
}


