#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, float x, float y, float rotation, float speed) {
    projectileSprite.setOrigin(projectileSprite.getLocalBounds().width / 2, 0);
    this->speed = speed;
    this->posX = x;
    this->posY = y;
    this->spawnedAt.x = x;
    this->spawnedAt.y = y;
    this->rotation = rotation;
    this->texturePath = texturePath;
    if (!this->projectileTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    this->projectileSprite.setPosition(posX, posY);
    this->projectileSprite.setRotation(rotation);
    this->projectileSprite.setTexture(projectileTexture);
}

void Projectile::render(sf::RenderWindow* window) {
    window->draw(this->projectileSprite);
}

sf::Sprite Projectile::getSprite() {
    return this->projectileSprite;
}

Projectile::~Projectile() {
    
}
