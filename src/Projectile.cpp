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

Disruptor::Disruptor(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    this->hitChance = 70;
    secondShot = false;
    secondShotDelay = 0;
    this->projectileSprite.setScale(4, 1);
}

Phaser::Phaser(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    this->hitChance = 100;
    this->projectileSprite.setScale(1, 0.25);
    phaserTimer = 0;
    phaserScaleX = 0;

    //hide sprite for testing
    //this->projectileSprite.setColor(sf::Color(255, 255, 255, 0));

    sf::Rect localBounds = projectileSprite.getLocalBounds();

    phaserRect = sf::RectangleShape(sf::Vector2f(localBounds.getSize().x, localBounds.getSize().y));
    phaserRect.setRotation(projectileSprite.getRotation());
    phaserRect.setPosition(projectileSprite.getPosition());

    phaserRect.setFillColor(sf::Color(255,255,0,128));

}

