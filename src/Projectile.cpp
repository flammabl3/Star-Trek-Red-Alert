#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) {
    projectileSprite.setOrigin(projectileSprite.getLocalBounds().width / 2, 0);
    friendly = false;
    this->speed = speed;
    this->spawnedAt.x = x;
    this->spawnedAt.y = y;
    this->directionOfTravel = directionOfTravel;
    this->damage = damage;
    this->texturePath = texturePath;
    if (!this->projectileTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    this->projectileSprite.setPosition(x, y);
    this->projectileSprite.setTexture(projectileTexture);
    
    this->touchingTarget = false;
    this->missed = false;
    this->hasPositionInitialized = false;
    this->hitChance = 100;
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

Torpedo::Torpedo(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    this->targetingSystem = false;
}


