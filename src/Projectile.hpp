#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>
#include <memory>

class Projectile {
    private:
        
    public:
        sf::Sprite projectileSprite;
        sf::Texture projectileTexture;
        float posX;
        float posY;
        sf::Vector2f spawnedAt;
        sf::Vector2f directionOfTravel;
        float speed;
        std::string texturePath;
        bool friendly;
        bool targetingSystem;
        bool missed;
        float damage;
        std::shared_ptr<sf::Vector2f> targetPos;
        sf::Vector2f targetPos2;
        sf::Vector2f lastCalculatedPosition;
        bool hasPositionInitialized;
        
        Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();

        void setFriendly();
        
        ~Projectile();
};

class Torpedo: Projectile {
    private:
        
    public:
        sf::Sprite projectileSprite;
        sf::Texture projectileTexture;
        float posX;
        float posY;
        sf::Vector2f spawnedAt;
        sf::Vector2f directionOfTravel;
        float speed;
        std::string texturePath;
        bool friendly;
        bool targetingSystem;
        bool missed;
        float damage;
        sf::Vector2f* targetPos;
        sf::Vector2f targetPos2;
        sf::Vector2f lastCalculatedPosition;
        bool hasPositionInitialized;
        
        Torpedo(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();

        void setFriendly();
        
        ~Torpedo();
};