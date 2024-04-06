#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) {
    projectileSprite.setOrigin(projectileSprite.getLocalBounds().width / 2, 0);
    friendly = false;
    this->speed = speed;
    this->posX = x;
    this->posY = y;
    this->spawnedAt.x = x;
    this->spawnedAt.y = y;
    this->directionOfTravel = directionOfTravel;
    this->damage = damage;
    this->texturePath = texturePath;
    if (!this->projectileTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    this->projectileSprite.setPosition(posX, posY);
    this->projectileSprite.setTexture(projectileTexture);
}

void Projectile::render(sf::RenderWindow* window) {
    window->draw(this->projectileSprite);
}

sf::Sprite Projectile::getSprite() {
    return this->projectileSprite;
}

void Projectile::setFriendly() {
    friendly = true;
}

Projectile::~Projectile() {
    
}
