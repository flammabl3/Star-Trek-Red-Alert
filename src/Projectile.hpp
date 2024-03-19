#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>

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
        
        Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();
        
        ~Projectile();
};