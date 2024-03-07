#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>

class Projectile {
    private:
        sf::Sprite projectileSprite;
        sf::Texture projectileTexture;
        int posX;
        int posY;
        std::string texturePath;
    public:
        Projectile(std::string texturePath, sf::Vector2f origin);
        void render(sf::RenderWindow* window);
        sf::Sprite getSprite();
};