#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, sf::Vector2f origin) {
    this->posX = origin.x;
    this->posY = origin.y;
    this->texturePath = texturePath;
    if (!this->projectileTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    this->projectileSprite.setPosition(origin.x / 2, origin.y);
    this->projectileSprite.setTexture(projectileTexture);
}

void Projectile::render(sf::RenderWindow* window) {
    window->draw(this->projectileSprite);
}

sf::Sprite Projectile::getSprite() {
    return this->projectileSprite;
}
