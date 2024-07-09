#include "System.hpp"

#ifndef SHIP_HPP
#define SHIP_HPP

class Ship
{
    private: // this will change based on the ship
        

    public:
        sf::Time time;
        sf::Clock clock;
        //SFML objects
        sf::Sprite shipSprite;
        sf::Texture shipTexture;
        sf::FloatRect boundingBox;
        sf::RectangleShape shieldRect;
        sf::Sprite shieldSprite;
        sf::Texture shieldTexture;
        sf::Vector2f shieldOffset;
        std::string shipSpritePath;


        //Internal data
        std::map<std::string, std::shared_ptr<System>> shipSystems;
        std::string state;
        int mass;
        float shields;
        float shieldsBase;
        float shieldBubbleRadius;
        int power;
        float impulseSpeed;
        float impulseSpeedBase;
        float warpSpeed;
        float warpSpeedBase;
        float selectedWarpLevel;
        float totalCondition;
        float totalConditionBase;
        double decisionTimer;
        int shieldOpac;
        bool friendly;
        float baseScale;
        bool warping;
        std::string name;
        std::string registry;
        std::string faction;
        //the int represents what number key will pick the weapon, the first string is the weapon type, the second is which System the weapon is linked to.
        std::map<int, std::tuple<std::string, std::string>> weaponsComplement;
        
        std::tuple<std::string, std::string> weaponSelectedTuple;
        int weaponSelected;

        //coordinate of a ship refers to its centre
        //std::shared_ptr<sf::Vector2f> position;
        float direction;
        
        
        //ships will occupy a rectangular space around their base coordinate.
        int length;
        int width;
        int height;

        Ship(std::map<std::string, std::shared_ptr<System>> shipSystems, std::string shipTextureName, float totalCondition, float warpSpeed, float shields, std::string name, std::string designation);

        Ship();

        void setSFMLObjects();

        sf::FloatRect getBoundingBox();

        void setSize(int l, int w, int h);

        void setDirection(float direction);

        void render(sf::RenderWindow* window);

        void setFaction(std::string factionName);

        void setFriendly();

        void calculateSystemPositions();

        void shieldHit(sf::Vector2f hitPos, bool recalcOffset);

        void shieldOpacMod();

        bool checkCollision(sf::Vector2f position);

        std::vector<std::string> checkDamage();

        sf::RectangleShape returnHitbox();

        std::vector<std::string> fireOxygenPersonnelSwap(sf::Time time);

        sf::RectangleShape setShield(int setShieldRadius);

        sf::Vector2f evadeTargetPosition;

        void changeTotalCondition(int damage);

        void changeTotalCondition(float damage);

        sf::Vector2f getPosition();
        
        //~Ship();
        //define destructor later
        
};

#endif