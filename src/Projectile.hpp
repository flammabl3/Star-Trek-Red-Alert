#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>
#include <memory>

class System;

class Projectile {
    private:
        
    public:
        sf::Sprite projectileSprite;
        sf::Texture projectileTexture;
        sf::Vector2f spawnedAt;
        sf::Vector2f directionOfTravel;
        float speed;
        std::string texturePath;
        bool friendly;
        bool touchingTarget;
        bool missed;
        float damage;
        float hitChance;
        bool hasPositionInitialized;
        
        Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();

        void setFriendly();
        
        ~Projectile();
};

class Torpedo: public Projectile {
    private:
        
    public:
        bool targetingSystem;
        System* targetSystemObj;
        sf::Vector2f targetPos;
        std::string targetSystem;
        sf::Vector2f lastCalculatedPosition;
        
        Torpedo(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage);
        
};