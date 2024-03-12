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
        float rotation;
        float speed;
        std::string texturePath;
        Projectile(std::string texturePath, float x, float y, float rotation, float speed);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();
        
        ~Projectile();
};